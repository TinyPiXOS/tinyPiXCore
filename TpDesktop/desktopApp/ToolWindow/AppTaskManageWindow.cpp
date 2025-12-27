#include "AppTaskManageWindow.h"
#include "TpImage.h"
#include "DeskTopGlobal.hpp"
#include "TpGraphicsBlurEffect.h"
#include "TpDesktopAPI.h"

#ifndef TASK_MANAGER_COLOR
#define TASK_MANAGER_COLOR _RGBA(255, 255, 255, 150)
#endif

// 上下边界距离和任务水平/垂直间距
static const uint32_t topBottomMargin = TpDisplay::dp2Px(35);
static const uint32_t taskHInterval = TpDisplay::dp2Px(63);
static const uint32_t taskVInterval = TpDisplay::dp2Px(35);

AppTaskManageWindow::AppTaskManageWindow()
    : TpDialog("tinyPiX_SYS_Float_0531acbf04")
{
    this->setBackGroundColor(TASK_MANAGER_COLOR);
    // setBackGroundImage(TpImage(applicationDirPath() + "/../res/默认桌面背景1x.png"));

    // TpGraphicsBlurEffect btnBlurEffect;
    // btnBlurEffect.setBlurRadius(15);
    // setGraphicsEffect(btnBlurEffect);

    taskScrollPanel_ = new TpScrollPanel(this);
    taskScrollPanel_->installEventFilter(this);
    taskScrollPanel_->setScrollMode(false);

    clearAllBtn_ = new TpButton(this);
    clearAllBtn_->setProperty("type", "TaskManageClearButton");
    clearAllBtn_->setText("清除全部");
    connect(clearAllBtn_, onClicked, this, &AppTaskManageWindow::slotClearAllApp);

    clearAllBtn_->setStyleSheet(R"(TpButton[type="TaskManageClearButton"] { \
                                    height: 38dp; \
                                    width: 130dp; \
                                    font-size: 15dp;  \
                                    color: rgb(38, 38, 38);   \
                                    background-color: rgb(255, 255, 255); \
                                    border-radius: 32;    \
                                    }   \
                                    TpButton[type="TaskManageClearButton"]:hover {    \
                                        background-color: rgb(239, 239, 239); \
                                    })");

    setVisible(false);
}

AppTaskManageWindow::~AppTaskManageWindow()
{
}

void AppTaskManageWindow::setVisible(bool visible)
{
    TpDialog::setVisible(visible);

    if (!visible)
        return;

    // 清空上一次的任务列表
    taskScrollPanel_->clearObject();
    for (const auto &lastTaskWidget : allTaskWidgetMap_)
    {
        lastTaskWidget.second->deleteLater();
    }
    allTaskWidgetMap_.clear();

    // 获取所有应用列表
    // std::cout << "查询APP信息前++++++++" << std::endl;
    TpVector<AppRunnerManage::RunAppInfo> TpRunAppList = AppRunnerManage::Instance()->runAppInfoList();
    // std::cout << "查询APP信息后-------------" << std::endl;

    for (int i = 0; i < TpRunAppList.size(); ++i)
    {
        AppRunnerManage::RunAppInfo appInfo = TpRunAppList.at(i);

        AppPreviewWidget *previewWidget = new AppPreviewWidget();
        previewWidget->setName(appInfo.appInfo.appName());
        previewWidget->setIcon(appInfo.appInfo.iconPath());
        previewWidget->setAppUuid(appInfo.appInfo.appUuid());

        // 应用抓图，grabWindow
        TpImage appGrapImage = AppRunnerManage::Instance()->appImage(appInfo.appInfo.appUuid());
        previewWidget->setPreviewImg(appGrapImage);

        connect(previewWidget, signalKillApp, this, &AppTaskManageWindow::slotKillApp);
        connect(previewWidget, signalOpenApp, this, &AppTaskManageWindow::slotOpenApp);

        allTaskWidgetMap_[appInfo.appInfo.appUuid()] = previewWidget;

        uint32_t taskBtnXPos = taskHInterval + (i / 2) * (taskWidth_ + taskHInterval);
        uint32_t taskBtnYPos = taskVInterval + (i % 2) * (taskHeight_ + taskVInterval);

        previewWidget->setRect(taskBtnXPos, taskBtnYPos, taskWidth_, taskHeight_);

        taskScrollPanel_->addObject(previewWidget);
    }
}

bool AppTaskManageWindow::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseKeyEvent = dynamic_cast<TpMouseEvent *>(event);
        if (!mouseKeyEvent)
            return false;

        onMousePressEvent(mouseKeyEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpMouseEvent *mouseKeyEvent = dynamic_cast<TpMouseEvent *>(event);
        if (!mouseKeyEvent)
            return false;

        onMouseRleaseEvent(mouseKeyEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_MOVE_TYPE)
    {
        TpMouseEvent *mouseMotionEvent = dynamic_cast<TpMouseEvent *>(event);
        if (!mouseMotionEvent)
            return false;

        onMouseMoveEvent(mouseMotionEvent);
    }
    else
    {
    }

    return TpDialog::eventFilter(watched, event);
}

bool AppTaskManageWindow::onMousePressEvent(TpMouseEvent *event)
{
    mousePressPoint_ = event->globalPos();

    return true;
}

bool AppTaskManageWindow::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpPoint curPoint = event->globalPos();
    if (std::abs(curPoint.x() - mousePressPoint_.x()) < 5 && std::abs(curPoint.y() - mousePressPoint_.y()) < 5)
    {
        setVisible(false);
        update();
    }

    return true;
}

bool AppTaskManageWindow::onMouseMoveEvent(TpMouseEvent *event)
{
    return true;
}

bool AppTaskManageWindow::onResizeEvent(TpResizeEvent *event)
{
    TpDialog::onResizeEvent(event);

    taskScrollPanel_->setRect(0, 0, width(), height() - topBottomMargin - clearAllBtn_->height());

    // 根据屏幕大小，计算每个任务缩略图大小；减去上下边界，以及三行缩略中间的间隔
    taskHeight_ = 1.0 * (height() - topBottomMargin * 6 - clearAllBtn_->height() - taskVInterval) / 2;
    taskWidth_ = TpDisplay::dp2Px(260);

    int32_t btnX = (width() - clearAllBtn_->width()) / 2.0;
    clearAllBtn_->move(btnX, height() - topBottomMargin - clearAllBtn_->height());

    return true;
}

bool AppTaskManageWindow::onLeaveEvent(TpLeaveEvent *event)
{
    return true;
}

void AppTaskManageWindow::slotClearAllApp(bool)
{
    AppRunnerManage::Instance()->killAllApp();

    // 清除界面
    TpVector<TpWidget *> objList = taskScrollPanel_->children();
    taskScrollPanel_->clearObject();
    for (auto &childAppObj : objList)
    {
        childAppObj->deleteLater();
    }
    allTaskWidgetMap_.clear();
    update();
}

void AppTaskManageWindow::slotKillApp(const TpString &uuid)
{
    if (allTaskWidgetMap_.contains(uuid))
    {
        taskScrollPanel_->delObject(allTaskWidgetMap_[uuid]);
        allTaskWidgetMap_[uuid]->deleteLater();
        allTaskWidgetMap_.erase(uuid);

        AppRunnerManage::Instance()->killApp(uuid);
        std::cout << "移除应用 ： " << uuid << std::endl;

        update();
    }
    else
    {
        std::cout << "未找到要移除的应用 ： " << uuid << std::endl;
    }
}

void AppTaskManageWindow::slotOpenApp(const TpString &uuid)
{
    AppRunnerManage::Instance()->startApp(uuid);
}
