#include "NavigationBar.h"
#include "TpPainter.h"
#include "SystemInfo/TpDisplay.h"
#include "TpAnimation.h"
#include "DeskTopGlobal.hpp"
#include "TpApp.h"
#include "TpAppManager.h"
#include "StatusBar.h"
#include "TpGraphicsBlurEffect.h"

NavigationBar::NavigationBar()
    : TpDialog("tinyPiX_SYS_Float_0531acbf04")
{
    const uint32_t navigationLineWidth = globalMainScreen_->width() * 0.3;
    const uint32_t navigationLineHeight = TpDisplay::dp2Px(6);

    lineLabel_ = new TpLabel(this);
    lineLabel_->setFixedSize(navigationLineWidth, navigationLineHeight);
    lineLabel_->setEnabledBorderColor(false);
    lineLabel_->setEnableBackGroundColor(true);
    lineLabel_->setBackGroundColor(_RGB(255, 255, 255));
    lineLabel_->setRoundCorners(navigationLineHeight / 2.0);
    lineLabel_->installEventFilter(this);
    lineLabel_->setWindowOpacity(0.6);

    // 导航线上下各10像素
    setSize(navigationLineWidth, TpDisplay::dp2Px(20));
    setBackGroundColor(_RGBA(255, 255, 255, 0));
    // setBackGroundColor(_RGB(255, 255, 255));

    lastAnimationTime_ = TpTime::currentTime();
}

NavigationBar::~NavigationBar()
{
}

void NavigationBar::setStyle(StatusBarStyle style)
{
    if (style == StatusBarStyle::Black)
    {
        lineLabel_->setBackGroundColor(_RGB(88, 88, 88));
    }
    else if (style == StatusBarStyle::White)
    {
        lineLabel_->setBackGroundColor(_RGB(255, 255, 255));
    }
    else
    {
    }
}

void NavigationBar::setColor(const int32_t &appColor)
{
}

bool NavigationBar::onResizeEvent(TpResizeEvent *event)
{
    TpDialog::onResizeEvent(event);

    int32_t lienY = (height() - lineLabel_->height()) / 2.0;
    lineLabel_->move(0, lienY);

    return true;
}

bool NavigationBar::onMousePressEvent(TpMouseEvent *event)
{
    TpDialog::onMousePressEvent(event);

    mousePressPoint_ = event->globalPos();
    mousePressTime_ = TpTime::currentTime();

    // 抓取当前显示应用截图，设置为多任务窗口背景图
    // if (globalAppTaskWindow)
    // {
    //     IPiWFSurface *deskSurface = tinyPiX_sys_get_process_surface(globalAgent, getpid());
    //     TpImage newBgImage;
    //     newBgImage.load(deskSurface);

    //     globalAppTaskWindow->setBackGroundImage(newBgImage);

    //     TpGraphicsBlurEffect btnBlurEffect;
    //     btnBlurEffect.setBlurRadius(150);
    //     globalAppTaskWindow->setGraphicsEffect(btnBlurEffect);
    //     globalAppTaskWindow->setEnableGraphicsEffect(true);
    // }

    return true;
}

bool NavigationBar::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpDialog::onMouseRleaseEvent(event);

    // std::cout << "NavigationBar::onMouseRleaseEvent " << std::endl;
    TpPoint curPos = event->globalPos();
    if (std::abs(curPos.x() - mousePressPoint_.x()) < 5 && std::abs(curPos.y() - mousePressPoint_.y()) < 5)
    {
        TpTime curTime = TpTime::currentTime();
        int64_t animationTimeInterval = lastAnimationTime_.msecsTo(curTime);
        lastAnimationTime_ = curTime;

        // std::cout << " 动画间隔 ： " << animationTimeInterval << std::endl;

        if (animationTimeInterval > 500)
        {
            int32_t lienY = (height() - lineLabel_->height()) / 2.0;
            lineLabel_->move(0, lienY);

            TpAnimation *moveAnimation = new TpAnimation(lineLabel_, TpAnimation::Pos);
            moveAnimation->setStartValue(lineLabel_->pos());
            moveAnimation->setKeyValueAt(0.5, TpPoint(0, 0));
            moveAnimation->setEndValue(lineLabel_->pos());
            moveAnimation->setDuration(500);
            moveAnimation->start();
        }
    }
    else
    {
        TpTime mouseReleaseTime = TpTime::currentTime();
        int32_t msTime = mousePressTime_.msecsTo(mouseReleaseTime);

        int32_t offsetY = mousePressPoint_.y() - curPos.y();

        // std::cout << "Press Hour minut " << mousePressTime_.hour() << "  " << mousePressTime_.minute() << "  " << mousePressTime_.second() << "  " << mousePressTime_.msec() << std::endl;
        // std::cout << "mouseReleaseTime Hour " << mouseReleaseTime.hour() << "  " << mouseReleaseTime.minute() << "  " << mouseReleaseTime.second() << "  " << mouseReleaseTime.msec() << std::endl;
        // std::cout << "msTime " << msTime << std::endl;

        // std::cout << "Press Y : " << mousePressPoint_.y() << std::endl;
        // std::cout << "Motion Y : " << curPos.y() << std::endl;
        // std::cout << "offsetY : " << offsetY << std::endl;

        if (offsetY > 4)
        {
            if (msTime > 500)
            {
                // 如果滑动间隔在 650 ms内，则为返回桌面，否则为打开任务管理器
                // if (!globalAppTaskWindow->visible())
                {
                    globalAppTaskWindow->setWindowOpacity(1);
                    globalAppTaskWindow->showMaximum();
                }

                std::cout << " Show Task Manage " << std::endl;
            }
            else
            {
                globalAppTaskWindow->setWindowOpacity(0);
                globalAppTaskWindow->setVisible(false);
                TpAppManager::Instance()->home();
                globalStatusBar_->setColor(_RGBA(0, 0, 0, 0));
                std::cout << " Return Desktop " << std::endl;
            }
        }
    }

    return true;
}

bool NavigationBar::onMouseMoveEvent(TpMouseEvent *event)
{
    TpDialog::onMouseMoveEvent(event);

    if (!event->state())
        return true;

    TpPoint curPos = event->globalPos();
    int32_t offsetY = mousePressPoint_.y() - curPos.y();

    if (globalAppTaskWindow /*&& !tpFuzzyCompare(globalTopSettingBar_->windowOpacity(), 1.0f)*/ && event->state())
    {
        TpPoint curMousePos = event->globalPos();

        if (offsetY > 0)
        {
            // globalAppTaskWindow->setWindowOpacity(offsetY * 0.1);
            // globalAppTaskWindow->showMaximum();
        }
    }

    // TpPoint curPos = event->globalPos();
    // int32_t offsetY = mousePressPoint_.y() - curPos.y();

    return true;
}

bool NavigationBar::onLeaveEvent(TpLeaveEvent *event)
{
    TpDialog::onLeaveEvent(event);

    if (event->leave())
    {
        lineLabel_->setWindowOpacity(1);
    }
    else
    {
        lineLabel_->setWindowOpacity(0.6);
    }
    return true;
}

bool NavigationBar::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_MOVE_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMouseMoveEvent(mouseEvent);
    }
    else
    {
    }

    return false;
}