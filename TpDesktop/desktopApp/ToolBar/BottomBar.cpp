#include "BottomBar.h"
#include "SystemInfo/TpDisplay.h"

#ifndef BOTTOM_CONTENT_MARGIN
#define BOTTOM_CONTENT_MARGIN TpDisplay::dp2Px(24)
#endif

#ifndef BOTTOM_BAR_COLOR
// #define BOTTOM_BAR_COLOR _RGBA(255, 255, 255, 51)
#define BOTTOM_BAR_COLOR _RGBA(255, 255, 255, 80)
// #define BOTTOM_BAR_COLOR _RGBA(255, 255, 255, 128)
#endif

BottomBar::BottomBar(TpScreen *topScreen)
    : TpWidget(topScreen), topScreen_(topScreen)
{
    setEnabledBorderColor(false);
    setBackGroundColor(BOTTOM_BAR_COLOR);
}

BottomBar::~BottomBar()
{
}

void BottomBar::setVisible(bool visible)
{
    // std::cout << "BottomBar::setVisible " << visible << std::endl;
    TpWidget::setVisible(visible);
}

void BottomBar::setBottomAppBtn(const TpList<DesktopAppButton *> &appList)
{
    systemAppBtnList_ = appList;

    for (const auto &bottomAppBtn : systemAppBtnList_)
    {
        bottomAppBtn->setParent(this);
        bottomAppBtn->setTextVisible(false);
    }

    caculateBottomAppPos();
    update();
}

TpList<DesktopAppButton *> BottomBar::bottomAppBtnList()
{
    // TODO: 在此处插入 return 语句
    return systemAppBtnList_;
}

void BottomBar::removeApp(DesktopAppButton *removeBtn)
{
    for (const auto &curApp : systemAppBtnList_)
    {
        if (curApp == removeBtn)
        {
            // 卸载APP
            systemAppBtnList_.remove(curApp);
            caculateBottomAppPos();
            update();
            break;
        }
    }
}

bool BottomBar::onResizeEvent(TpResizeEvent *event)
{
    return true;
}

bool BottomBar::onMoveEvent(TpMoveEvent *event)
{
    caculateBottomAppPos();

    return true;
}

bool BottomBar::onMousePressEvent(TpMouseEvent *event)
{
    return true;
}

bool BottomBar::onMouseRleaseEvent(TpMouseEvent *event)
{
    return true;
}

void BottomBar::caculateBottomAppPos()
{
    uint32_t bottomAppCount = systemAppBtnList_.size();

    uint32_t appAllWidth = bottomAppCount * APP_WIDTH_HEIGHT + (bottomAppCount - 1) * BOTTOM_CONTENT_MARGIN;

    uint32_t appStartX = (BOTTOM_BAR_WIDTH - appAllWidth) / 2.0;
    uint32_t appStartY = (BOTTOM_BAR_HEIGHT - APP_WIDTH_HEIGHT) / 2.0;

    // 因为按钮是以顶层窗口为父类，所以坐标要叠加bittomBar的XY坐标
    for (int32_t i = 0; i < systemAppBtnList_.size(); ++i)
    {
        DesktopAppButton *bottomAppBtn = systemAppBtnList_.at(i);
        bottomAppBtn->move(appStartX + (APP_WIDTH_HEIGHT + BOTTOM_CONTENT_MARGIN) * i, appStartY);
    }
}
