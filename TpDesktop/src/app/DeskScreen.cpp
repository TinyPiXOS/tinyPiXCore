#include "DeskScreen.h"
#include "TpImage.h"
#include "TpEvent.h"
#include "DeskTopGlobal.hpp"
#include "TpJsonDocument.h"
#include "TpFile.h"
#include "TpDir.h"
#include "TpFont.h"
#include "TpProcess.h"
#include "TpMessageBox.h"
#include "TpAppConfigIO.h"
#include "TpAppManager.h"
#include <InteractData/TpDesktopData.h>

#include <iostream>

#ifndef APP_CONFIG_SON_PATH
#define APP_CONFIG_SON_PATH "conf/app/"
#endif

#ifndef APP_FILES_SON_PATH
#define APP_FILES_SON_PATH "app/"
#endif

IPiSysApiAgent *globalAgent = tinyPiX_sys_create();

SysLockWindow *globalSysLockWindow = nullptr;
AppTaskManageWindow *globalAppTaskWindow = nullptr;
AppSettingBar *globalTopSettingBar_ = nullptr;

TpScreen *globalMainScreen_ = nullptr;
StatusBar *globalStatusBar_ = nullptr;
NavigationBar *globalNavigationBar_ = nullptr;

uint32_t globalAppMaxRow = 4;
uint32_t globalAppMaxColumn = 6;
uint32_t globalAppHInterval = 0;

static inline void BAR_SET_ATTRIB(TpWidget *vars, int32_t x, int32_t y, int32_t width, int32_t height)
{
    vars->setRect(x, y, width, height);
    vars->setVisible(true);
    vars->show();
    vars->update();
}

DeskScreen::DeskScreen()
    : TpMainWindow(), pressAppBtn_(nullptr), isMoveMode_(false), installingApp_(nullptr)
{
    globalMainScreen_ = this;

    // 订阅数据
    initializeGateway();
    subscribeGatewayData(TpRunAppKey, this);

    setEnableBackGroundColor(false);
    setEnabledBorderColor(false);

    std::cout << "DeskScreen::construct()" << std::endl;

    initData();

    // 绑定桌面长按事件
    // connect(mainAppPanel_, onLongPress, [=]()
    // 		{ slotOperateApp(nullptr); });

    // 此处应该读取配置文件中的设置文件，可以保留上次设置的背景图片

    this->setBackGroundImage(TpImage(applicationDirPath() + "/../res/默认桌面背景1x.png"), false);

    // 此处应该读取配置文件中的设置文件，加载配置参数

    // 刷新工具栏位置坐标
    refreshBar();

    // 刷新APP显示主窗体尺寸和坐标
    refreshMainAppPanel();

    // 计算可显示APP的行列数
    // APP显示区域的宽度
    // APP显示区域是沾满了整个窗口的，要偏移预留空间，让APP图标显示在中间区域，与下部Bar对齐
    uint32_t mainAppPanelWidth = mainAppPanel_->rect().width();
    uint32_t mainAppPanelHeight = mainAppPanel_->rect().height();
    uint32_t panelHMargin = (mainAppPanelWidth - BOTTOM_BAR_WIDTH) / 2.0;

    DesktopAppButton *testBtn = new DesktopAppButton();
    testBtn->font()->setFontSize(APP_FONT_SIZE);
    testBtn->setIconSize(APP_WIDTH_HEIGHT, APP_WIDTH_HEIGHT);

    int32_t btnWidth = testBtn->rect().width();
    int32_t btnHeight = testBtn->rect().height();

    globalAppHInterval = globalMainScreen_->width() * 0.06666;

    globalAppMaxRow = 1.0 * (mainAppPanelHeight + APP_V_INTERVAL) / (APP_V_INTERVAL + btnHeight);
    globalAppMaxColumn = 1.0 * (BOTTOM_BAR_WIDTH + globalAppHInterval) / (globalAppHInterval + btnWidth);

    testBtn->deleteLater();

    // 根据示例列数，精准计算一下 APP的 水平间隔
    globalAppHInterval = (BOTTOM_BAR_WIDTH - globalAppMaxColumn * btnWidth) / (globalAppMaxColumn - 1);
    // globalAppMaxRow = 4;
    // globalAppMaxColumn = 6;

    initDeskAppConfig();
}

DeskScreen::~DeskScreen()
{
    if (globalStatusBar_)
    {
        delete globalStatusBar_;
        globalStatusBar_ = nullptr;
    }
    if (bottomFloatBar_)
    {
        delete bottomFloatBar_;
        bottomFloatBar_ = nullptr;
    }
    if (globalNavigationBar_)
    {
        delete globalNavigationBar_;
        globalNavigationBar_ = nullptr;
    }
}

void DeskScreen::recvData(const char *topic, const void *data, const uint32_t &size)
{
    TpString topicString(topic);
    if (topicString.compare(TpRunAppKey) == 0)
    {
        TpRunApp recvRunData;
        recvRunData.StructDeserialize(data, size);

        std::cout << "RecvStartApp UUID : " << recvRunData.appUuid << std::endl;

        // 启动指定应用
        TpVector<TpString> argList;
        for (const auto &recvArg : recvRunData.argList)
        {
            std::cout << " recvArg : " << recvArg << std::endl;
            argList.emplace_back(recvArg);
        }
        TpAppManager::Instance()->killApp(recvRunData.appUuid);
        startApp(recvRunData.appUuid, argList);
    }
    else if (topicString.compare(TpAppInitFinishKey) == 0)
    {
        std::cout << "应用初始化完毕 : " << std::endl;
        splashScreenWin_->close();
    }
    else
    {
    }
}

bool DeskScreen::appChange(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require)
{
    int32_t sysid = this->objectSysID();

    // std::cout << "DeskScreen::appChange " << std::endl;
    std::cout << "id " << id << std::endl;
    std::cout << "sysid " << sysid << std::endl;

    // std::cout << "Desktop visible" << visible << std::endl;
    // std::cout << "Desktop active" << active << std::endl;

    if (sysid == id)
    {
        if (bottomFloatBar_)
            bottomFloatBar_->setVisible(true);
        if (globalStatusBar_)
            globalStatusBar_->setVisible(true);
        return false;
    }

    // 应用启动，不显示上下工具栏
    // bottomFloatBar_->setVisible(false);
    globalStatusBar_->setVisible(false);

    if (active == false)
    {
        // 桌面即将隐藏
        return false;
    }

    printf("id = %d \n", id);

    int32_t width = 0, height = 0;

    return true;
}

bool DeskScreen::onActiveEvent(TpActiveEvent *event)
{
    std::cout << "DeskScreen::onActiveEvent " << std::endl;

    if (event->isActived())
    {
        refreshBar();
    }
    else
    {
        if (bottomFloatBar_)
            bottomFloatBar_->setVisible(false);
    }

    return true;
}

bool DeskScreen::onMousePressEvent(TpMouseEvent *event)
{
    // 记录鼠标点击坐标
    // uint32_t pressY = event->globalPos().y();

    return true;
}

bool DeskScreen::onMouseRleaseEvent(TpMouseEvent *event)
{
    pressAppBtn_ = nullptr;
    isMoveMode_ = false;

    return false;
}

bool DeskScreen::onMouseMoveEvent(TpMouseEvent *event)
{
#if 0 // 暂时屏蔽长按移动图标功能
	if (isMoveMode_ && pressAppBtn_)
	{
		TpPoint curGlobalPoint = event->globalPos();
		// TpPoint curPoint = event->globalPos();

		// std::cout << "GlobalPos : " << curGlobalPoint.x() << "  " << curGlobalPoint.y() << std::endl;
		// std::cout << "curPoint : " << curPoint.x() << "  " << curPoint.y() << std::endl;
		// std::cout << "mainAppPanel_ Pos : " << mainAppPanel_->pos().x()() << "  " << mainAppPanel_->pos().y()() << std::endl;

		if (operateMenu_->visible())
			operateMenu_->close();
		if (maskWindow_->visible())
			maskWindow_->close();

		TpSize iconSize = pressAppBtn_->iconSize();
		pressAppBtn_->move(curGlobalPoint.x() - mainAppPanel_->pos().x()() - iconSize.w / 2.0, curGlobalPoint.y() - mainAppPanel_->pos().y()() - iconSize.h / 2.0);
		mainAppPanel_->update();
		// pressAppBtn_->move(curGlobalPoint.x(), curGlobalPoint.y());
	}
#endif

    return false;
}

bool DeskScreen::onLeaveEvent(TpLeaveEvent *event)
{
    // if (event->eventType() == TpEvent::EVENT_OBJECT_LEAVE_TYPE)
    // {
    //     if (event->leave())
    //     {
    //         pressTopBar_ = false;
    //         // update();
    //     }
    // }

    return true;
}

bool DeskScreen::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseKeyEvent = dynamic_cast<TpMouseEvent *>(event);
        if (!mouseKeyEvent)
            return false;

        // std::cout << "eventFilter MouseKey States : " << mouseKeyEvent->state() << std::endl;
        onMousePressEvent(mouseKeyEvent);

        if (isMoveMode_ && watched == mainAppPanel_)
        {
            std::cout << "Filter mainAppPanel_ " << std::endl;
            return true;
        }
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpMouseEvent *mouseKeyEvent = dynamic_cast<TpMouseEvent *>(event);
        if (!mouseKeyEvent)
            return false;

        // std::cout << "eventFilter MouseKey States : " << mouseKeyEvent->state() << std::endl;
        onMouseRleaseEvent(mouseKeyEvent);

        if (isMoveMode_ && watched == mainAppPanel_)
        {
            std::cout << "Filter mainAppPanel_ " << std::endl;
            return true;
        }
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_MOVE_TYPE)
    {
        TpMouseEvent *mouseMotionEvent = dynamic_cast<TpMouseEvent *>(event);
        if (!mouseMotionEvent)
            return false;

        onMouseMoveEvent(mouseMotionEvent);

        if (isMoveMode_ && watched == mainAppPanel_)
        {
            return true;
        }
    }
    else
    {
    }

    return false;
}

void DeskScreen::slotOperateApp(DesktopAppButton *operateBtn)
{
    return;

    if (operateBtn)
    {
        TpRect btnRect = operateBtn->toScreen();
        btnRect.setSize(TpSize(operateBtn->iconSize().width(), operateBtn->iconSize().height()));

        // 绘制全局遮罩层，只留长按的按钮显示
        maskWindow_->setAppRect(btnRect, operateBtn->roundCorners());
        maskWindow_->showMaximum();

        // uint32_t delIndex = operateMenu_->addItem("卸载", applicationDirPath() + "/../res/删除.png");

        operateMenu_->exec(btnRect.x(), btnRect.y() - TpDisplay::dp2Px(11) - operateMenu_->height());
    }
    else
    {
        // APP抖动
        TpList<TpObject *> appList = mainAppPanel_->objectList();
        for (const auto &appButton : appList)
        {
            DesktopAppButton *iconButton = dynamic_cast<DesktopAppButton *>(appButton);
            if (!iconButton)
                continue;

            // iconButton->setRotate();
        }
    }

    std::cout << " isMoveMode_ True " << std::endl;
    isMoveMode_ = true;
}

void DeskScreen::slotDeleteApp(DesktopAppButton *operateBtn)
{
    if (!operateBtn)
    {
        std::cout << "卸载应用对象获取失败!" << std::endl;
        return;
    }

    TpString removeUuid = operateBtn->property("UUID").toString();

    if (removeUuid.empty())
    {
        std::cout << "应用UUID获取失败!" << std::endl;
        return;
    }

    // 如果应用正在运行，先杀掉进程
    TpAppManager::Instance()->killApp(removeUuid);

    // 重置缓存操作按钮
    pressAppBtn_ = nullptr;

    // UI移除应用图标
    // 判断是不是底部工具栏的应用
    if (bottomFloatBar_->bottomAppBtnList().contains(operateBtn))
    {
        bottomFloatBar_->removeApp(operateBtn);

        for (const auto &bottomAppInfo : bottomBarAppList_)
        {
            if (bottomAppInfo->appUuid.compare(removeUuid) == 0)
            {
                bottomBarAppList_.remove(bottomAppInfo);
            }
        }
    }
    else
    {
        // 桌面移除卸载APP
        for (auto &allAppInfoIter : allAppInfoMap_)
        {
            for (auto &appInfo : allAppInfoIter.second)
            {
                if (appInfo->appUuid.compare(removeUuid) == 0)
                {
                    allAppInfoIter.second.remove(appInfo);
                    break;
                }
            }
        }

        // 滚动窗移除APP
        mainAppPanel_->delObject(operateBtn);

        // 内存泄漏
        // operateBtn->deleteLater();
    }

    // 卸载应用
    appInstallPtr_->remove(removeUuid);

    update();
}

void DeskScreen::slotTimeoutInstallApp()
{
    if (!installingApp_ || !appInstallPtr_)
        return;

    std::cout << "安装应用UUID" << appInstallPtr_->appUUID() << std::endl;

    int installSchedule = appInstallPtr_->installSchedule();
    std::cout << "安装进度： " << installSchedule << std::endl;

    // 更新安装进度UI
    installingApp_->setInstallProgress(installSchedule);
    if (installSchedule == 100)
    {
        // 安装完成
        appInstallTimer_->stop();
    }
}

void DeskScreen::initData()
{
    globalTopSettingBar_ = new AppSettingBar();
    if (globalTopSettingBar_ == nullptr)
    {
        std::cout << "topSettingBar init error!" << std::endl;
        std::exit(0);
    }
    globalTopSettingBar_->setVisible(false);

    globalStatusBar_ = new StatusBar();
    if (globalStatusBar_ == nullptr)
    {
        std::cout << "topbar init error!" << std::endl;
        std::exit(0);
    }
    globalStatusBar_->installEventFilter(this);

    bottomFloatBar_ = new BottomBar(this);
    if (bottomFloatBar_ == nullptr)
    {
        std::cout << "BottomBar init error!" << std::endl;
        std::exit(0);
    }
    bottomFloatBar_->installEventFilter(this);

    globalNavigationBar_ = new NavigationBar();
    if (globalNavigationBar_ == nullptr)
    {
        std::cout << "navigationBar_ init error!" << std::endl;
        std::exit(0);
    }
    globalNavigationBar_->installEventFilter(this);

    // globalSysLockWindow = new SysLockWindow();
    // if (globalSysLockWindow == nullptr)
    // {
    // 	std::cout << "SysLockWindow init error!" << std::endl;
    // 	std::exit(0);
    // }
    // globalSysLockWindow->setVisible(false);

    globalAppTaskWindow = new AppTaskManageWindow();
    if (globalAppTaskWindow == nullptr)
    {
        std::cout << "globalAppTaskWindow init error!" << std::endl;
        std::exit(0);
    }
    globalAppTaskWindow->setVisible(false);

    mainAppPanel_ = new MainAppScrollPanel(this);
    if (mainAppPanel_ == nullptr)
    {
        std::cout << "mainAppPanel_ init error!" << std::endl;
        std::exit(0);
    }
    mainAppPanel_->installEventFilter(this);

    carouselButton_ = new TpCarouselButton(this);
    carouselButton_->setFixedSize(150, 25);
    connect(mainAppPanel_, onPageChanged, [=](uint32_t curPage_)
            { carouselButton_->setCurrentIndex(curPage_); });

    // maskWindow_ = new AppOperateMaskWindow();
    // maskWindow_->installEventFilter(this);
    // maskWindow_->setVisible(false);

    // operateMenu_ = new TpMenu();
    // uint32_t delIndex = operateMenu_->addItem("卸载", applicationDirPath() + "/../res/删除.png");
    // connect(operateMenu_, onClicked, [=](uint32_t index)
    //         {
    // 			std::cout << " index " << index << std::endl;
    // 			std::cout << " delIndex " << delIndex << std::endl;

    // 			if (index == delIndex)
    // 			{
    // 				slotDeleteApp(pressAppBtn_);
    // 				// TpMessageBox::information("卸载成功");
    // 				operateMenu_->close();
    // 				maskWindow_->close();
    // 			} });

    appInstallPtr_ = new TpAppInstall("");
    appInstallTimer_ = new TpTimer(800);
    connect(appInstallTimer_, timeout, this, &DeskScreen::slotTimeoutInstallApp);

    splashScreenWin_ = new SplashScreen();
}

void DeskScreen::initDeskAppConfig()
{
    // 获取所有已安装的APP的UUID列表
    TpVector<TpString> installAppUuidList = TpAppConfigIO::installAppUuidList();

    // 应用数量为0；后边就不用处理了
    uint32_t appCount = installAppUuidList.size();
    if (appCount == 0)
        return;

    // 解析APP所在页和行列信息
    TpJsonObject appPageInfoJsonObj;

    TpFile appPageInfoFile(applicationDirPath() + "/../conf/appIndexConfig.json");
    appPageInfoFile.open(TpFile::ReadOnly);
    if (appPageInfoFile.isOpen())
    {
        TpString appPageInfoStr = appPageInfoFile.readAll();
        appPageInfoFile.close();

        // 转换json对象
        appPageInfoJsonObj = TpJsonDocument::fromJson(appPageInfoStr).object();
    }

    // 解析在BottomBar中的App
    TpVector<TpString> bottomBarAppUuidList;

    TpFile bottomBarAppInfoFile(applicationDirPath() + "/../conf/bottomBarAppConfig.json");
    bottomBarAppInfoFile.open(TpFile::ReadOnly);
    if (bottomBarAppInfoFile.isOpen())
    {
        TpString botomBarAppInfoJsonStr = bottomBarAppInfoFile.readAll();

        TpJsonObject bottomBarAppJson = TpJsonDocument::fromJson(botomBarAppInfoJsonStr).object();

        TpJsonArray bottomAppIdList = bottomBarAppJson.value("appList").toArray();
        for (int i = 0; i < bottomAppIdList.count(); ++i)
        {
            TpString bottomUuid = bottomAppIdList.at(i).toString();
            bottomBarAppUuidList.emplace_back(bottomUuid);
        }

        bottomBarAppInfoFile.close();
    }

    curPage_ = 0;

    uint32_t pageMaxAppCount = globalAppMaxRow * globalAppMaxColumn;

    // 上一页放不下的APP
    TpList<ApplicationInfoSPtr> cachePageAppList;

    for (int32_t i = 0; i < installAppUuidList.size(); ++i)
    {
        TpString curAppUuid = installAppUuidList.at(i);

        // 查询该应用是否有配置页数和行列信息
        if (appPageInfoJsonObj.contains(curAppUuid))
        {
            TpJsonObject curAppPageInfoObj = appPageInfoJsonObj.value(curAppUuid).toObject();
            uint32_t appPage = curAppPageInfoObj.value("page").toUint();
            uint32_t appIndex = curAppPageInfoObj.value("index").toUint();

            std::cout << "appUuid: " << curAppUuid << "  page: " << appPage << "  index : " << appIndex << std::endl;

            ApplicationInfoSPtr appInfoSPtr = std::make_shared<ApplicationInfo>();
            appInfoSPtr->appUuid = curAppUuid;
            appInfoSPtr->page = appPage;
            appInfoSPtr->index = appIndex;

            auto &curPageAppList = allAppInfoMap_[appPage];

            if (curPageAppList.size() >= pageMaxAppCount)
            {
                cachePageAppList.emplace_back(appInfoSPtr);
            }
            else
            {
                allAppInfoMap_[appPage].insertData(appIndex, appInfoSPtr);
            }
        }
        else if (bottomBarAppUuidList.contains(curAppUuid))
        {
            // 查询该APP是否是放置在bottomBar的
            ApplicationInfoSPtr appInfoSPtr = std::make_shared<ApplicationInfo>();
            appInfoSPtr->appUuid = curAppUuid;
            appInfoSPtr->page = 0;
            appInfoSPtr->index = 0;

            bottomBarAppList_.emplace_back(appInfoSPtr);
            // bottomFloatBar_->addBottomApp(appInfoSPtr);
        }
        else
        {
            // 当前应用，配置文件中没有记录页数和行列信息；放入缓存容器
            ApplicationInfoSPtr appInfoSPtr = std::make_shared<ApplicationInfo>();
            appInfoSPtr->appUuid = curAppUuid;
            appInfoSPtr->page = 0;
            appInfoSPtr->index = 0;
            cachePageAppList.emplace_back(appInfoSPtr);
        }
    }

    // 放置冗余的APP
    for (auto &cacheAppPtr : cachePageAppList)
    {
        uint32_t currentPage = cacheAppPtr->page;

        // 寻找足够空间的页
        while (allAppInfoMap_[currentPage].size() >= pageMaxAppCount)
        {
            ++currentPage;
        }

        auto &newPageApps = allAppInfoMap_[currentPage];

        uint32_t findIndex = findAppIndex(newPageApps);

        // 将APP移动到新页
        cacheAppPtr->page = currentPage;
        cacheAppPtr->index = findIndex;
        newPageApps.emplace_back(cacheAppPtr);
    }

    // 计算当前APP共有多少页
    uint32_t pageMaxNum = 0;
    for (const auto &appIter : allAppInfoMap_)
    {
        if (appIter.first > pageMaxNum)
            pageMaxNum = appIter.first;
    }

    mainAppPanel_->setMaxPage(pageMaxNum + 1);
    carouselButton_->setCount(pageMaxNum + 1);

    createAppBtn();

    // 指定页码刷新
    refreshAppPage(curPage_);
}

void DeskScreen::createAppBtn()
{
    uint32_t pageMaxAppCount = globalAppMaxRow * globalAppMaxColumn;

    TpString appConfigDirPath = appConfigPathStr_ + APP_CONFIG_SON_PATH;
    TpString appFileDirPath = appConfigPathStr_ + APP_FILES_SON_PATH;

    // APP显示区域的宽度
    uint32_t mainAppPanelWidth = mainAppPanel_->rect().width();
    uint32_t mainAppPanelHeight = mainAppPanel_->rect().height();

    // APP显示区域是沾满了整个窗口的，要偏移预留空间，让APP图标显示在中间区域，与下部Bar对齐
    uint32_t panelHMargin = (mainAppPanelWidth - BOTTOM_BAR_WIDTH) / 2.0;

    // 最后一个按钮的指针，用于获取按钮的宽高
    DesktopAppButton *finalBtn = nullptr;

    // 将每一页的APP都添加到滚动窗口，按页码计算X偏移量
    for (const auto &pageAppInfoIter : allAppInfoMap_)
    {
        uint32_t appPage = pageAppInfoIter.first;
        TpList<ApplicationInfoSPtr> curPageAppList = pageAppInfoIter.second;

        for (const auto &appInfoSptrIter : curPageAppList)
        {
            // 解析应用图标、名称信息
            TpAppConfigIO configIO(appInfoSptrIter->appUuid);

            DesktopAppButton *appBtn = createDeskAppBtn(appInfoSptrIter, configIO.iconPath(), configIO.appName());
            if (!appBtn)
                continue;

            finalBtn = appBtn;
        }
    }

    // 计算一页里面，所有行列的XY坐标
    if (finalBtn)
    {
        TpVector<int32_t> rowYList;
        TpVector<int32_t> columnXList;

        for (int row = 0; row < globalAppMaxRow; ++row)
        {
            uint32_t appY = row * (APP_V_INTERVAL + finalBtn->rect().height());
            rowYList.emplace_back(appY);

            if (row == 0)
            {
                for (int column = 0; column < globalAppMaxColumn; ++column)
                {
                    // pageNum * mainAppPanelWidth + appX
                    uint32_t appX = panelHMargin + (column % globalAppMaxColumn) * (globalAppHInterval + finalBtn->rect().width());
                    columnXList.emplace_back(appX);
                }
            }
        }

        caculateAppPosPtr_.setRowY(rowYList);
        caculateAppPosPtr_.setColumnX(columnXList);
    }

    // 创建工具栏的图标
    TpList<DesktopAppButton *> bottomAppList;
    for (const auto &bottomAppInfo : bottomBarAppList_)
    {
        // 解析应用图标、名称信息
        TpAppConfigIO configIO(bottomAppInfo->appUuid);

        DesktopAppButton *appBtn = configAppBtn(bottomAppInfo->appUuid, configIO.iconPath(), configIO.appName());
        bottomAppList.emplace_back(appBtn);
    }
    bottomFloatBar_->setBottomAppBtn(bottomAppList);

    mainAppPanel_->recal(true);
}

void DeskScreen::refreshMainAppPanel()
{
    uint32_t tHeight = globalStatusBar_->height();
    uint32_t bottomBarHeight = bottomFloatBar_->height();

    // 计算APP显示区域大小位置
    uint32_t appDisplayHeight = height() - tHeight - bottomBarHeight;

    // uint32_t appIntervalPx = appDisplayHeight * 0.0279;

    // 减去距离上部Bar和下部bar的距离，在减去bottomBar距离边界距离
    uint32_t mainAppPanelHeight = height() - tHeight - bottomBarHeight - TpDisplay::dp2Px(20) * 2 - TpDisplay::dp2Px(19);

    mainAppPanel_->setWidth(rect().width());
    mainAppPanel_->setHeight(mainAppPanelHeight);
    mainAppPanel_->move(0, MAIN_PANEL_TOP_DISTANCE + tHeight);

    mainAppPanel_->setScrollMode(!isLandscapeScreen_);

    mainAppPanel_->recal(true);
    mainAppPanel_->update();

    uint32_t carouselButtonX = (width() - carouselButton_->width()) / 2.0;
    uint32_t carouselButtonY = mainAppPanel_->pos().y() + mainAppPanel_->height() - carouselButton_->height();

    // std::cout << "carouselButtonX " << carouselButtonX << "  carouselButtonY" << carouselButtonY << std::endl;
    // std::cout << "carouselButtonW " << carouselButton_->width() << "  carouselButtonH" << carouselButton_->height() << std::endl;

    carouselButton_->move(carouselButtonX, carouselButtonY);
}

void DeskScreen::refreshAppPage(const uint32_t &pageNum)
{
}

void DeskScreen::refreshBar()
{
    if (!globalStatusBar_ || !bottomFloatBar_)
        return;

    // 计算底部bar的X坐标
    uint32_t bottomX = (width() - BOTTOM_BAR_WIDTH) / 2.0;
    uint32_t bottomY = height() - BOTTOM_BAR_HEIGHT - TpDisplay::dp2Px(19);

    bottomFloatBar_->setRoundCorners(13);

    BAR_SET_ATTRIB(globalStatusBar_, 0, 0, width(), TOP_BAR_HEIGHT);
    BAR_SET_ATTRIB(bottomFloatBar_, bottomX, bottomY, BOTTOM_BAR_WIDTH, BOTTOM_BAR_HEIGHT);

    uint32_t navigationX = (width() - globalNavigationBar_->width()) / 2.0;
    BAR_SET_ATTRIB(globalNavigationBar_, navigationX, height() - globalNavigationBar_->height(), globalNavigationBar_->width(), globalNavigationBar_->height());

    globalStatusBar_->update();
    globalNavigationBar_->update();
}

DesktopAppButton *DeskScreen::createDeskAppBtn(ApplicationInfoSPtr appInfo, const TpString &iconPath, const TpString &appName)
{
    DesktopAppButton *appBtn = configAppBtn(appInfo->appUuid, iconPath, appName);
    if (!appBtn)
        return nullptr;

    // APP显示区域的宽度
    uint32_t mainAppPanelWidth = mainAppPanel_->rect().width();
    uint32_t mainAppPanelHeight = mainAppPanel_->rect().height();

    // APP显示区域是沾满了整个窗口的，要偏移预留空间，让APP图标显示在中间区域，与下部Bar对齐
    uint32_t panelHMargin = (mainAppPanelWidth - BOTTOM_BAR_WIDTH) / 2.0;

    uint32_t appIndex = appInfo->index;
    uint32_t appPage = appInfo->page;

    TpRect iconButtonRect = appBtn->rect();

    uint32_t curAppRow = (appIndex / globalAppMaxColumn);

    uint32_t appX = appPage * mainAppPanelWidth + panelHMargin + (appIndex % globalAppMaxColumn) * (globalAppHInterval + iconButtonRect.width());
    uint32_t appY = curAppRow * (APP_V_INTERVAL + iconButtonRect.height());

    appBtn->move(appX, appY);

    appBtn->installEventFilter(mainAppPanel_);
    mainAppPanel_->addObject(appBtn);

    return appBtn;
}

DesktopAppButton *DeskScreen::configAppBtn(const TpString &appUuid, const TpString &iconPath, const TpString &appName)
{
    // 根据APP uuid去查询App配置文件
    DesktopAppButton *appBtn = new DesktopAppButton(iconPath, appName, nullptr);
    appBtn->setProperty("UUID", appUuid);

    appBtnMap_[appUuid] = appBtn;

    connect(appBtn, onClicked, [=](bool)
            { startApp(appUuid); });

    connect(appBtn, onLongPress, [=]()
            { slotOperateApp(appBtn); });

    connect(appBtn, onPressed, [=]()
            { pressAppBtn_ = appBtn; pressBtnRect_ = appBtn->rect(); });

    appBtn->setEnableBackGroundColor(false);
    appBtn->setEnabledBorderColor(false);
    appBtn->setVisible(true);
    appBtn->setRoundCorners(13);

    appBtn->font()->setFontColor(_RGB(255, 255, 255));
    appBtn->font()->setFontSize(APP_FONT_SIZE);

    appBtn->setIconSize(APP_WIDTH_HEIGHT, APP_WIDTH_HEIGHT);

    return appBtn;
}

void DeskScreen::startApp(const TpString &uuid, const TpVector<TpString> &argList)
{
    // 是否是系统安装包
    if (uuid.compare("TinyPiXSytemAppInstallUuid") == 0)
    {
        if (argList.size() == 0)
        {
            std::cout << "未收到安装文件路径 " << std::endl;
            return;
        }

        installApp(argList.front());
        return;
    }

    // 启动开屏动画
    // splashScreenWin_->showSplashScreen(appBtnMap_.value(uuid)->rect());

    // 在RecvData中接收应用启动完成的消息，然后关闭开屏动画

    // 启动应用
    bool startRes = TpAppManager::Instance()->startApp(uuid, argList);
    std::cout << "应用启动结果： " << startRes << std::endl;
    if (!startRes)
    {
        // 启动失败，终止开屏界面
        splashScreenWin_->close();
    }

    // splashScreenWin_->close();
}

void DeskScreen::installApp(const TpString &pkgPath)
{
    // 应用安装
    appInstallPtr_->setPath(pkgPath);
    TpString installAppUuid = appInstallPtr_->appUUID();

    // 判断应用是否已安装
    TpVector<TpString> instasllAppIDList = TpAppConfigIO::installAppUuidList();
    if (instasllAppIDList.contains(installAppUuid))
    {
        // TpMessageBox::information("应用已安装!");
        return;
    }

    // 查找应用安装的桌面索引
    int32_t installPageNum = -1;
    uint32_t installIndex = 0;

    uint32_t pageMaxAppCount = globalAppMaxRow * globalAppMaxColumn;
    for (const auto &pageAppInfoIter : allAppInfoMap_)
    {
        // 当前页应用已放置满
        if (pageAppInfoIter.second.size() >= pageMaxAppCount)
            continue;

        installPageNum = pageAppInfoIter.first;
        installIndex = findAppIndex(pageAppInfoIter.second);
        break;
    }

    // 所有页都被装满了，开辟新页
    if (installPageNum == -1)
    {
        installPageNum = 0;

    ReFindPageNum:
        for (const auto &pageAppInfoIter : allAppInfoMap_)
        {
            if (pageAppInfoIter.first == installPageNum)
            {
                installPageNum++;
                goto ReFindPageNum;
            }
        }
    }

    appInstallPtr_->install();

    std::cout << "开始安装应用： " << installAppUuid << std::endl;

    ApplicationInfoSPtr installAppInfp = tpMakeShared<ApplicationInfo>();
    installAppInfp->appUuid = installAppUuid;
    installAppInfp->index = installIndex;
    installAppInfp->page = installPageNum;

    allAppInfoMap_[installPageNum].emplace_back(installAppInfp);

    // 桌面添加应用图标以及遮罩层
    TpString iconPath = appInstallPtr_->icon();
    TpString appName = appInstallPtr_->appName();

    std::cout << "Install iconPath" << iconPath << std::endl;
    std::cout << "Install appName" << appName << std::endl;

    installingApp_ = createDeskAppBtn(installAppInfp, iconPath, appName);
    installingApp_->setInstallProgress(0);

    // 启动定时器更新安装进度
    appInstallTimer_->start();
}

uint32_t DeskScreen::findAppIndex(const TpList<ApplicationInfoSPtr> &pageAppList)
{
    uint32_t findIndex = 0;
    if (pageAppList.size() == 0)
        return findIndex;

    // 收集所有已使用的索引值
    std::set<uint32_t> usedIndices;
    for (const auto &appInfo : pageAppList)
    {
        usedIndices.insert(appInfo->index);
    }

    // 查找最小未使用索引
    while (true)
    {
        // 如果当前候选值不在已使用索引中，即找到最小值
        if (usedIndices.find(findIndex) == usedIndices.end())
        {
            return findIndex;
        }
        findIndex++;

        // 防止整数溢出
        if (findIndex == std::numeric_limits<uint32_t>::max())
        {
            break;
        }
    }

    return findIndex;
}
