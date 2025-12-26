#include "SysLockWindow.h"
#include "TpImage.h"
#include "TpString.h"
#include "TpFont.h"
#include "DeskTopGlobal.hpp"

SysLockWindow::SysLockWindow()
    : TpDialog(), mouseLeftPress_(false)
{
    initUi();

    setVisible(false);
}

SysLockWindow::~SysLockWindow()
{
}

void SysLockWindow::setVisible(bool visible)
{
    if (visible)
    {
        // TODO 显示时，刷新一下锁屏壁纸
        TpString lockBgImgPath = applicationDirPath() + "/../res/controlPanel/lock_background.png";

        // TODO,显示时，刷新用户logo

        this->setBackGroundImage(TpImage(lockBgImgPath), true);
        refreshLockWindow(true);

        slotUpdateSystemTime();
        updateTimetimer_->start();
    }
    else
    {
        updateTimetimer_->stop();
    }

    TpDialog::setVisible(visible);
}

void SysLockWindow::resizeLockWindow(const TpRect &mainWindowRect)
{
    this->setRect(0, 0, mainWindowRect.width(), mainWindowRect.height());

#if 1 // 壁纸界面组件
    TpFont *dateTimeFont = systemDateTimeLabel_->font();
    systemDateTimeLabel_->setRect((mainWindowRect.width() - dateTimeFont->pixelWidth()) / 2.0, mainWindowRect.height() * 0.3, dateTimeFont->pixelWidth() + 10, dateTimeFont->pixelHeight());

    unlockDirectLabel_->setRect((mainWindowRect.width() - 60) / 2.0, mainWindowRect.height() * 0.6, 60, 60);

    TpFont *promptFont = unlockPromptLabel_->font();
    unlockPromptLabel_->setRect((mainWindowRect.width()- promptFont->pixelWidth()) / 2.0, mainWindowRect.height() * 0.75, promptFont->pixelWidth() + 5, promptFont->pixelHeight());

#endif

#if 1 // 登陆界面组件
    TpFont *systemNameFont = systemNameLabel_->font();
    systemNameLabel_->setRect((mainWindowRect.width() - systemNameFont->pixelWidth()) / 2.0, mainWindowRect.height() * 0.2, systemNameFont->pixelWidth() + 5, systemNameFont->pixelHeight());

    TpFont *welcomeFont = welcomeLabel_->font();
    welcomeLabel_->setRect((mainWindowRect.width() - welcomeFont->pixelWidth()) / 2.0, mainWindowRect.height() * 0.35, welcomeFont->pixelWidth() + 5, welcomeFont->pixelHeight());

    userIconLabel_->setRect((mainWindowRect.width() - 60) / 2.0, mainWindowRect.height() * 0.5, 60, 60);

    //
    uint32_t pwdLoginInterval = 30;
    uint32_t editWidth = mainWindowRect.width() * 0.3;
    pwdEdit_->setRect((mainWindowRect.width() - editWidth - pwdLoginInterval - 60) / 2.0, mainWindowRect.height() * 0.65, editWidth, 50);

    loginBtn_->setRect(pwdEdit_->rect().x() + editWidth + pwdLoginInterval, mainWindowRect.height() * 0.65, 50, 50);
#endif
}

bool SysLockWindow::onMousePressEvent(TpMouseEvent *event)
{
    // 记录鼠标点击坐标
    mouseLeftPress_ = event->state();
    pressPoint_ = event->globalPos();

    return true;
}

bool SysLockWindow::onMouseRleaseEvent(TpMouseEvent *event)
{
    // 记录鼠标点击坐标
    mouseLeftPress_ = event->state();

    return true;
}

bool SysLockWindow::onMouseMoveEvent(TpMouseEvent *event)
{
    if (mouseLeftPress_)
    {
        TpPoint curMousePos = event->globalPos();

        if ((curMousePos.y() - pressPoint_.y()) <= -5)
        {
            // 触发一次上拉事件后，不再重复触发
            mouseLeftPress_ = false;

            refreshLockWindow(false);
        }
    }

    return true;
}

bool SysLockWindow::onLeaveEvent(TpLeaveEvent *event)
{
    // if (event->eventType() == TpEvent::EVENT_OBJECT_LEAVE_TYPE)
    // {
    //     if (event->leave())
    //     {
    //         mouseLeftPress_ = false;

    //         refreshLockWindow(true);
    //     }
    // }

    return true;
}

void SysLockWindow::initUi()
{
    systemDateTimeLabel_ = new TpLabel(this);
    systemDateTimeLabel_->font()->setFontForeColor(_RGB(255, 255, 255));
    systemDateTimeLabel_->font()->setFontSize(50);
    systemDateTimeLabel_->font()->setBold(true);
    systemDateTimeLabel_->setText("15 : 30");

    unlockDirectLabel_ = new TpLabel(this);
    unlockDirectLabel_->setBackGroundImage(TpImage(applicationDirPath() + "/../res/双上箭头白.png"));

    unlockPromptLabel_ = new TpLabel(this);
    unlockPromptLabel_->font()->setFontForeColor(_RGB(255, 255, 255));
    unlockPromptLabel_->font()->setFontSize(20);
    unlockPromptLabel_->font()->setBold(true);
    unlockPromptLabel_->setText("向上滑动或按任意键解锁");

    updateTimetimer_ = new TpTimer(60000);
    connect(updateTimetimer_, timeout, [=]()
            { slotUpdateSystemTime(); });

    // 输入密码界面控件
    // 输密码解锁界面
    systemNameLabel_ = new TpLabel(this);
    systemNameLabel_->font()->setFontForeColor(_RGB(255, 255, 255));
    systemNameLabel_->font()->setFontSize(50);
    systemNameLabel_->font()->setBold(true);
    systemNameLabel_->setText("TinyPiX OS");

    welcomeLabel_ = new TpLabel(this);
    welcomeLabel_->font()->setFontForeColor(_RGB(255, 255, 255));
    welcomeLabel_->font()->setFontSize(20);
    welcomeLabel_->font()->setBold(true);
    welcomeLabel_->setText("欢迎使用");

    userIconLabel_ = new TpLabel(this);
    userIconLabel_->setBackGroundImage(TpImage(applicationDirPath() + "/../res/用户.png"));

    pwdEdit_ = new TpLineEdit(this);

    loginBtn_ = new TpButton(applicationDirPath() + "/../res/登录.png", "登录", this);
    loginBtn_->setButtonStyle(TpButton::IconOnly);
    loginBtn_->setEnabledBorderColor(false);
    loginBtn_->setEnableBackGroundColor(false);
    connect(loginBtn_, onClicked, [=](bool)
            { slotLogin(); });
}

void SysLockWindow::refreshLockWindow(const bool &windowStatus)
{
    std::cout << "refreshLockWindow : " << windowStatus << std::endl;

    // 锁屏壁纸界面提示信息；时间、解锁提示、解锁方向提示
    systemDateTimeLabel_->setVisible(windowStatus);
    unlockPromptLabel_->setVisible(windowStatus);
    unlockDirectLabel_->setVisible(windowStatus);

    // 输密码解锁界面
    systemNameLabel_->setVisible(!windowStatus);
    welcomeLabel_->setVisible(!windowStatus);
    userIconLabel_->setVisible(!windowStatus);
    pwdEdit_->setVisible(!windowStatus);
    loginBtn_->setVisible(!windowStatus);

    update();
}

void SysLockWindow::slotLogin()
{
    // TODO,验证登录密码

    globalSystemLockStatus = false;
    this->setVisible(false);

    update();
}

void SysLockWindow::slotUpdateSystemTime()
{
    // 获取当前时间
    time_t rawtime;
    time(&rawtime);

    // 将time_t转换为本地时间
    struct tm *timeinfo;
    timeinfo = localtime(&rawtime);

    // 定义输出字符串
    char buffer[80];

    // 格式化时间
    strftime(buffer, 80, "%H : %M", timeinfo);
    TpString systemTimeStr(buffer);

    systemDateTimeLabel_->setText(systemTimeStr);
    systemDateTimeLabel_->update();
}
