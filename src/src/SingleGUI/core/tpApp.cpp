#include "tpApp.h"
#include "tpObject.h"
#include "tpClipboard.h"
#include "tpMessage.h"
#include "tpAutoObject.h"
#include "tpScreen.h"
#include "tpConfig.h"
#include "tpThread.h"
#include "tpTimer.h"
#include "tpMD5.h"
#include "tpDefaultCss.h"
#include "tpEvent.h"
#include "tpDef.h"
#include "tpChildWidget.h"
#include "tpSurface.h"
#include "tpVirtualKeyboard.h"
#include "tpMap.h"

#include <tinyPiXApi.h>
#include <mutex>
#include <unistd.h>
#include <getopt.h>
#include <functional>
#include <iostream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <thread>
#include <queue>

#define PROCESS_MAX_NAME_LENGTH 1024

class appExe;

struct ItpProcessInfo
{
	int32_t id;
	pid_t pid;
	char process[PROCESS_MAX_NAME_LENGTH];
};

struct ItpAppSet
{
	// 主线程ID
	std::thread::id mainThreadId;

	tpList<tpObject *> objectList;
	std::map<tpObject *, bool> vReserveMap;
	// 所有floatscreen列表，用于更新主题样式
	tpList<tpChildWidget *> floatScreenList;

	std::mutex gMutex;

	tpChildWidget *vScreen;

	tpClipboard *clipboard;

	tpMessage *message;

	appExe *thread;

	int32_t eventType;
	ItpProcessInfo pInfo;
	// ItpConfigSet appConfigurationSet;

	bool running;
	bool waitRun;

	tinyPiX::SystemTheme systemTheme = tinyPiX::Default;
	tpShared<tpCssParser> cssParser_ = tpMakeShared<tpCssParser>();

	// 全局唯一单例虚拟键盘
	tpVirtualKeyboard *virtualKeyboard = nullptr;
	tpChildWidget *curInputObj = nullptr;

	std::mutex queueSlotMutex_;
	std::queue<std::function<void()>> tasks_;
};

class appExe : public tpThread
{
public:
	appExe() : tpThread() {};

	appExe(tpApp *app) : tpThread()
	{
		theApp = app;
	};

	virtual ~appExe() {

	};

	virtual void run()
	{
		ItpAppSet *set = (ItpAppSet *)theApp->appObjectSet();
		ItpUserEvent message;
		bool ret = false;
		if (!set)
			return;

		while (true)
		{
			if (set->vScreen == nullptr)
			{
				break;
			}

			ret = set->message->recvWait(&message);
			if (!ret)
				continue;

			switch (message.type)
			{
			case tpApp::TP_REGISTER_ACT:
			{
				// add to objectList
				set->gMutex.lock();
				tpChildWidget *childWidgetObj = (tpChildWidget *)message.user_data0;
				if (childWidgetObj)
				{
					switch (childWidgetObj->objectType())
					{
					case TP_FLOAT_OBJECT:
					{
						set->vReserveMap[childWidgetObj] = childWidgetObj->visible();
					}
					break;
					}
					set->objectList.push_back(childWidgetObj);
					set->gMutex.unlock();
				}
			}
			break;
			case tpApp::TP_DELETE_ACT:
			{
				tpObject *object = (tpObject *)message.user_data0;

				if (object == nullptr)
				{
					continue;
				}

				set->gMutex.lock();

				if (object)
				{
					std::map<tpObject *, bool>::iterator mapiter = set->vReserveMap.find(object);

					if (mapiter != set->vReserveMap.end())
					{
						set->vReserveMap.erase(mapiter);
					}

					auto objFindIter = std::find(set->objectList.begin(), set->objectList.end(), object);
					if (objFindIter != set->objectList.end())
					{
						set->objectList.remove(*objFindIter);
					}

					auto floatFindIter = std::find(set->floatScreenList.begin(), set->floatScreenList.end(), object);
					if (floatFindIter != set->floatScreenList.end())
					{
						set->floatScreenList.remove(*floatFindIter);
					}

					// ItpObjectSet *vScreenObjDaata = (ItpObjectSet *)set->vScreen->objectSets();
					// vScreenObjDaata->tmp.deleteObject(object);

					if (object == set->vScreen)
					{
						goto finished;
					}
				}
			deleted:
				set->gMutex.unlock();

				delete object;
				object = nullptr;
				// set->vScreen->update();
			}
			break;
			case tpApp::TP_ABORT_ACT:
			{
			finished:
				goto appover;
			}
			break;
			case tpApp::TP_RETURN_ACT:
			{
				tpObject *vScreen = set->vScreen;

				if (vScreen == message.user_data0)
				{
					tpScreen *screenObj = static_cast<tpScreen *>(vScreen);

					// exclude desktop
					if (screenObj->objectLayer() != TP_WM_DESK)
					{
						if (vScreen)
						{
							screenObj->setVisible(false);
						}

						set->gMutex.lock();

						std::map<tpObject *, bool>::iterator iter = set->vReserveMap.begin();
						for (; iter != set->vReserveMap.end(); iter++)
						{
							tpChildWidget *tmp = static_cast<tpChildWidget *>(iter->first);
							iter->second = tmp->visible();
							tmp->setVisible(false);
						}

						set->gMutex.unlock();
					}
				}
			}
			break;
			case tpApp::TP_ACTIVE_ACT:
			{
				bool actived = message.user_code;
				tpObject *object = (tpObject *)message.user_data0;

				tpObject *vScreen = set->vScreen;

				if (((tpScreen *)vScreen)->objectLayer() != TP_WM_DESK)
				{
					set->gMutex.lock();

					if (actived)
					{
						std::map<tpObject *, bool>::iterator mapiter = set->vReserveMap.begin();
						for (; mapiter != set->vReserveMap.end(); mapiter++)
						{
							tpChildWidget *tmp = static_cast<tpChildWidget *>(mapiter->first);

							if (tmp != set->vScreen)
							{
								tmp->setVisible(mapiter->second);
							}
						}
					}
					else
					{
						std::map<tpObject *, bool>::iterator mapiter = set->vReserveMap.begin();
						for (; mapiter != set->vReserveMap.end(); mapiter++)
						{
							tpChildWidget *tmp = static_cast<tpChildWidget *>(mapiter->first);

							if (tmp != set->vScreen)
							{
								mapiter->second = tmp->visible();
								tmp->setVisible(false);
							}
						}
					}

					set->gMutex.unlock();
				}
			}
			break;
			}
		}
	appover:
		set->running = false;

		if (set->waitRun == false)
		{
			exit(0);
		}
	};

private:
	tpApp *theApp;
};

static tpApp *appInst = nullptr;

static inline bool hold_app_second_run(const char *runPath, const char *uuid)
{
	int32_t fd;
	int32_t lock_result;
	struct flock lock;
	char pFileName[PATH_MAX] = {0};
	sprintf(pFileName, "%s/.%s", runPath, uuid);

	fd = open(pFileName, O_RDWR | O_CREAT, 0644);

	if (fd < 0)
	{
		return true;
	}

	lock_result = lockf(fd, F_TEST, 0);

	if (lock_result < 0)
	{
		return true;
	}

	lock_result = lockf(fd, F_LOCK, 0);

	if (lock_result < 0)
	{
		return true;
	}

	return false;
}

static inline bool decide_run_once(const char *appName)
{
	char tempPath[PATH_MAX] = {0};
	char *currentPath = get_current_dir_name();

	if (currentPath == nullptr)
	{
		return false;
	}

	sprintf(tempPath, "%s/%s", currentPath, appName);
	const char *md5 = tpMD5::getnerateMD5(tempPath, strlen(tempPath));

	if (md5 == nullptr)
	{
		return false;
	}

	return hold_app_second_run(currentPath, md5);
}

static inline bool check_digitals(char *args)
{
	if (args == NULL)
	{
		return false;
	}

	int32_t length = strlen(args), i;
	char key;

	if (length == 0)
	{
		return false;
	}

	for (i = 0; i < length; ++i)
	{
		key = args[i];

		if (key < '0' ||
			key > '9')
		{
			return false;
		}
	}

	return true;
}

static inline bool parseArgs(ItpAppSet *set, int32_t argc, char *argv[])
{
	bool ret = false;
	char ch;

	while ((ch = getopt(argc, argv, "p:i:n")) != -1)
	{
		switch (ch)
		{
		case 'p':
		{
			ret = check_digitals(optarg);

			if (ret)
			{
				set->pInfo.id = atoi(optarg);
			}
		}
		break;
		case 'i':
		{
			ret = check_digitals(optarg);

			if (ret)
			{
				set->pInfo.pid = atoi(optarg);
			}
		}
		break;
		case 'n':
		{
			int32_t length = strlen(optarg);

			if (length > 0)
			{
				memcpy(set->pInfo.process, optarg, length);
			}
		}
		break;
		default:
			return false;
			break;
		}
	}

	return true;
}

static void SendThemeChangedEvent(ItpAppSet *setData, const tinyPiX::SystemTheme &sysTheme)
{
	tpThemeChangeEvent *themeEvent = new tpThemeChangeEvent();

	tpString cssFilePath = parseThemeFile(sysTheme);

	// 在 app的run函数中，调用主题改变事件函数，通知所有组件
	tpChildWidget *screenWidget = dynamic_cast<tpChildWidget *>(setData->vScreen);
	if (screenWidget)
	{
		// 初始化CSS样式表
		// screenWidget->setStyleSheet(cssFilePath);
		screenWidget->onThemeChangeEvent(themeEvent);
		screenWidget->update();
	}

	for (const auto &floatScrenPtr : setData->floatScreenList)
	{
		// floatScrenPtr->setStyleSheet(cssFilePath);
		floatScrenPtr->onThemeChangeEvent(themeEvent);
		floatScrenPtr->update();
	}

	delete themeEvent;
	themeEvent = nullptr;
}

static void InitVirtualKeyboard(ItpAppSet *set)
{
	if (set->virtualKeyboard)
		return;

	set->virtualKeyboard = new tpVirtualKeyboard();

	// 初始化虚拟键盘相关
	connect(set->virtualKeyboard, inputPinyin, [=](const tpString &pinyin)
			{
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(tinyPiX::Pinyin, pinyin);
				} });
	connect(set->virtualKeyboard, finishChinese, [=](const tpString &chinese)
			{
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(tinyPiX::Chinese, chinese);
				} });
	connect(set->virtualKeyboard, deleteSymbol, [=]()
			{
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(tinyPiX::Delete, "");
				} });
	connect(set->virtualKeyboard, inputCharacter, [=](const tpString &character)
			{
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(tinyPiX::Symbol, character);
				} });
}

tpApp::tpApp(int32_t argc, char *argv[])
{
	ItpAppSet *set = new ItpAppSet();

	bool ret = decide_run_once(argv[0]);

	if (ret)
	{
		std::exit(0);
	}

	set->mainThreadId = std::this_thread::get_id();

	set->clipboard = tpClipboard::Inst();
	set->vScreen = nullptr;
	set->message = new tpMessage();

	set->pInfo.id = TP_INVALIDATE_VALUE;
	set->pInfo.pid = TP_INVALIDATE_VALUE;
	set->eventType = TP_DIS_NONE;

	set->running = true;
	set->waitRun = false;

	set->thread = new appExe(this);

	memset(set->pInfo.process, 0, PROCESS_MAX_NAME_LENGTH);
	parseArgs(set, argc, argv);

	tpAutoObject::Inst()->autoFreeObject = true;

	if (appInst)
	{
		std::cout << "detects app instance more once!, exit......" << std::endl;
		std::exit(0);
	}

	appInst = this;
	this->appSet = set;

	// APP创建，解析初始CSS样式
	tpString cssFilePath = parseThemeFile(set->systemTheme);
	set->cssParser_->parseCss(cssFilePath);
}

tpApp::~tpApp()
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;

	if (set)
	{
		if (set->clipboard)
		{
			delete set->clipboard;
		}

		if (set->message)
		{
			delete set->message;
		}

		if (set->thread)
		{
			delete set->thread;
		}

		set->vReserveMap.clear();

		delete set;
	}
}

tpApp *tpApp::Inst()
{
	return appInst;
}

bool tpApp::bindVScreen(tpObject *object)
{
	ItpAppSet *set = static_cast<ItpAppSet *>(this->appSet);
	bool ret = false;

	if (!set)
		return false;

	if (!object)
		return false;

	tpChildWidget *vScreenPtr = dynamic_cast<tpChildWidget *>(object);
	if (!vScreenPtr)
		return false;

	if (object->objectType() != TP_TOP_OBJECT)
	{
		std::cout << "bind screen type error !" << std::endl;
		return false;
	}

	if (set->vScreen)
	{
		std::cout << "bind screen only once !" << std::endl;
		return false;
	}

	ret = (set->vScreen != object);

	if (ret)
	{
		set->gMutex.lock();
		set->vScreen = vScreenPtr;
		set->gMutex.unlock();
	}

	set->thread->start();

	return ret;
}

bool tpApp::run()
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;

	if (set)
	{
		set->waitRun = true;

		if (set->vScreen == nullptr)
		{
			return false;
		}

		{
			// std::unique_lock<std::mutex> lock(set->queueSlotMutex_);
			
			while (!set->tasks_.empty())
			{
				auto task = set->tasks_.front();
				set->tasks_.pop();
				// lock.unlock();
				task();
				// lock.lock();
			}
		}

		while (set->running)
		{
			tpTimer::sleep(20);
		}
	}

	return set->running;
}

tpClipboard *tpApp::clipboard()
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;
	tpClipboard *clipboard = nullptr;

	if (set)
	{
		clipboard = set->clipboard;
	}

	return clipboard;
}

tpChildWidget *tpApp::vScreen()
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;
	tpChildWidget *vScreen = nullptr;

	if (set)
	{
		vScreen = set->vScreen;
	}

	return vScreen;
}

tpShared<tpCssParser> tpApp::cssParser()
{
	ItpAppSet *set = static_cast<ItpAppSet *>(this->appSet);
	return set->cssParser_;
}

void tpApp::setStyle(const tinyPiX::SystemTheme &style)
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;

	if (set->systemTheme != style)
	{
		set->systemTheme = style;

		// app run起来之后才下发主题切换事件，在run的时候已经解析过了
		if (set->waitRun)
			SendThemeChangedEvent(set, style);
	}
}

tinyPiX::SystemTheme tpApp::style()
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;

	return set->systemTheme;
}

tpShared<tpSurface> tpApp::grabWindow()
{
	// ItpAppSet *set = (ItpAppSet *)this->appSet;

	// tinyPiX_sys_capture_screen();
	return tpShared<tpSurface>();
}

void tpApp::wakeUpVirtualKeyboard(tpChildWidget *object)
{
	if (!object)
		return;

	ItpAppSet *set = (ItpAppSet *)this->appSet;

	if (set->virtualKeyboard == nullptr)
		InitVirtualKeyboard(set);

	set->curInputObj = object;
	set->virtualKeyboard->show();
}

void tpApp::dormantVirtualKeyboard()
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;
	set->curInputObj = nullptr;
	set->virtualKeyboard->close();
}

bool tpApp::isExistObject(tpObject *object, bool autoRemove)
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;
	bool ret = false;

	if (object == nullptr)
	{
		return false;
	}

	if (set)
	{
		set->gMutex.lock();
		std::list<tpObject *> *curList = &set->objectList;

		auto iter = std::find_if(curList->begin(), curList->end(), [object](const tpObject *obj)
								 { return (object == obj); });

		if (iter != curList->end())
		{
			if (autoRemove)
			{
				curList->erase(iter);
			}
			ret = true;
		}

		set->gMutex.unlock();
	}

	return ret;
}

bool tpApp::isMainThread()
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;
	return std::this_thread::get_id() == set->mainThreadId;
}

void tpApp::postEvent(std::function<void()> task)
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;

	if (!set->running)
		return;

	{
		std::unique_lock<std::mutex> lock(set->queueSlotMutex_);
		set->tasks_.push(task);
	}
}

bool tpApp::sendRegister(tpObject *object)
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;
	bool registerObject = false;

	if (!set)
		return registerObject;

	if (object == nullptr)
		return registerObject;

	if (object->objectType() == TP_FLOAT_OBJECT)
	{
		tpChildWidget *floatScreenWidget = dynamic_cast<tpChildWidget *>(object);
		if (floatScreenWidget)
			set->floatScreenList.emplace_back(floatScreenWidget);
	}

	ItpUserEvent message;
	message.type = TP_REGISTER_ACT;
	message.user_data0 = object;

	registerObject = set->message->sendWait(&message);

	return registerObject;
}

bool tpApp::sendDelete(tpObject *object)
{
	if (!object)
		return false;

	ItpAppSet *set = static_cast<ItpAppSet *>(this->appSet);
	bool deleteObject = false;

	if (!set)
		return false;

	ItpUserEvent message;
	message.type = TP_DELETE_ACT;
	message.user_data0 = object;

	bool sendRes = set->message->sendWait(&message);

	return sendRes;
}

bool tpApp::sendReturn(tpObject *object)
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;
	bool returnAct = false;

	if (set)
	{
		returnAct = (object != nullptr);

		if (returnAct)
		{
			if (object->objectType() != TP_TOP_OBJECT)
			{
				return false;
			}

			ItpUserEvent message;
			message.type = TP_RETURN_ACT;
			message.user_data0 = object;

			returnAct = set->message->sendWait(&message);
		}
	}

	return returnAct;
}

bool tpApp::sendActive(tpObject *object, bool actived)
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;
	bool beActived = false;

	if (set)
	{
		beActived = (object != nullptr);

		if (beActived)
		{
			if (object->objectType() != TP_TOP_OBJECT)
			{
				return false;
			}

			ItpUserEvent message;
			message.type = TP_ACTIVE_ACT;

			message.user_data0 = object;
			message.user_code = actived;
			actived = set->message->sendWait(&message);
		}
	}

	return beActived;
}

bool tpApp::sendAbort(tpObject *object)
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;
	bool abort = false;

	if (set)
	{
		abort = (object != nullptr);

		if (abort)
		{
			ItpUserEvent message;
			message.type = TP_ABORT_ACT;
			message.user_data0 = object;

			abort = set->message->sendWait(&message);
		}
	}

	return abort;
}

void tpApp::setDisableEventType(int32_t type)
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;

	if (set)
	{
		set->eventType = type;
	}
}

ItpAppData *tpApp::appObjectSet()
{
	return (ItpAppSet *)this->appSet;
}

int32_t tpApp::disableEventType()
{
	ItpAppSet *set = (ItpAppSet *)this->appSet;
	int32_t type = 0;

	if (set)
	{
		type = set->eventType;
	}

	return type;
}
