
#ifndef __TP_VAPP_H
#define __TP_VAPP_H

#include <tpUtils.h>
#include <string>

TP_DEF_VOID_TYPE_VAR(IPitpApp);

class tpObject;
class tpClipboard;
class tpChildWidget;
// class tpMessage;
class tpCssParser;
class tpSurface;

class tpApp
{
public:
	// disable or enable some event for object
	enum
	{
		TP_DIS_ALL = 0xffffffff,
		TP_DIS_KEYBOARD = 0x01,
		TP_DIS_MOUSE = 0x02, // only disable mouse key down and up
		TP_DIS_MOTION = 0x04,
		TP_DIS_FINGER = 0x08,
		TP_DIS_DOLLAR = 0x10,
		TP_DIS_GESTURE = 0x20,
		TP_DIS_NONE = 0,
	};
	// object require some action by message
	enum
	{
		TP_REGISTER_ACT, // top obejct will be register
		TP_DELETE_ACT,	 // except VScreen, other only delete object
		TP_ABORT_ACT,	 // abort this app
		TP_RETURN_ACT,	 // hide VScreen
		TP_ACTIVE_ACT,
	};

public:
	tpApp(int32_t argc, char *argv[]);
	virtual ~tpApp();

public:
	/// @brief 获取tpApp全局单例指针
	/// @return 指针对象
	static tpApp *Inst();

public:
	/// @brief 绑定应用主窗体
	/// @param object 主窗体对象指针
	/// @return 绑定结果
	virtual bool bindVScreen(tpObject *object);
	/// @brief 开启tpApp主事件循环
	/// @return 启动结果
	virtual bool run();

public:
	virtual tpClipboard *clipboard();

	/// @brief 获取当前程序主窗体
	/// @return 主窗体指针
	virtual tpChildWidget *vScreen();

	/// @brief 获取全局单例CSS解析器
	/// @return css解析器智能指针
	tpShared<tpCssParser> cssParser();

	/// @brief 设置系统主题配色,设置后会更新所有UI的样式，高频率调用会造成卡顿
	/// @param style 主题值
	void setStyle(const tinyPiX::SystemTheme &style);

	/// @brief 获取系统主题类型
	/// @return 系统主观类型
	tinyPiX::SystemTheme style();

	/// @brief 获取当前应用界面抓图;暂未实现
	/// @return 图片资源对象
	tpShared<tpSurface> grabWindow();

	/// @brief 唤醒虚拟键盘
	/// @return object 唤醒对象；虚拟键盘的输入将会给入该对象
	void wakeUpVirtualKeyboard(tpChildWidget* object);

	/// @brief 休眠虚拟键盘
	void dormantVirtualKeyboard();

public:
	virtual bool isExistObject(tpObject *object, bool autoRemove = false);

public:
	virtual bool sendRegister(tpObject *object);
	virtual bool sendDelete(tpObject *object);
	virtual bool sendAbort(tpObject *object);
	virtual bool sendReturn(tpObject *object);
	virtual bool sendActive(tpObject *object, bool actived); // only for top object type

public:
	virtual void setDisableEventType(int32_t type);
	virtual int32_t disableEventType();
	virtual IPitpApp *appObjectSet();

public:
	IPitpApp *appSet;
};

#endif
