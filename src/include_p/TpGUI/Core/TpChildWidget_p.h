#ifndef __TP_CHILDWIDGET_PRIVATE_H
#define __TP_CHILDWIDGET_PRIVATE_H

#include "TpApp.h"
#include "TpMessage.h"
#include "TpDef.h"
#include "TpVector.h"
#include "TpVariant.h"
#include "TpEvent.h"
#include "TpRect.h"
#include "TpLayout.h"
#include "TpPainter.h"
#include "TpPoint.h"
#include "Core/TpObjectFunction.hpp"
#include "TpGlobal.h"
#include "TpDefaultCss.h"
#include "TpScreen.h"
#include "TpVirtualKeyboard.h"
#include "TpImage.h"
#include "thorVG/thorvg.h"
#include "TpSurface.h"
#include "tinyPiXUtils.h"
#include "tinyPiXApi.h"
#include "TpApp_p.h"

#include <png.h>
#include <unordered_map>
#include <mutex>
#include <thread>

struct TpWidgetCssData
{
    tpShared<TpCssData> enabledCssData;
    tpShared<TpCssData> pressCssData;
    tpShared<TpCssData> hoverCssData;
    tpShared<TpCssData> checkedCssData;
    tpShared<TpCssData> disabledCssData;

    tvg::Scene *tvgScene = nullptr;
    tvg::SwCanvas *swCanvas = nullptr;

    TpWidgetCssData()
    {
        enabledCssData = nullptr;
        pressCssData = nullptr;
        hoverCssData = nullptr;
        checkedCssData = nullptr;
        disabledCssData = nullptr;
    }
};

#if 0
// 根据桌面工具栏高度，处理窗体Y坐标
static int32_t processDeskTopBarY(TpWidget *dealWidget, int32_t originY)
{
    if (!dealWidget)
        return originY;

    // 检查当前对象类型
    Tp::ItpObjectType objType = dealWidget->objectType();
    bool isTopOrFloat = (objType == Tp::TP_TOP_OBJECT || objType == Tp::TP_FLOAT_OBJECT);

    // 检查父对象类型
    bool parentIsTop = false;
    if (!isTopOrFloat)
    {
        if (TpWidget *parentPtr = dynamic_cast<TpWidget *>(dealWidget->parent()))
        {
            parentIsTop = (parentPtr->objectType() == Tp::TP_TOP_OBJECT);
        }
    }

    // 合并条件并执行偏移计算
    if (isTopOrFloat || parentIsTop)
    {
        TpAppData *appData = static_cast<TpAppData *>(TpApp::Inst()->appObjectSet());
        if (!appData->isDesk && appData->desktopBarInfo_.topBarisVislble)
        {
            originY += appData->desktopBarInfo_.topBarHeight;
        }
    }

    return originY;
}

// 将考虑桌面工具栏的Y坐标转换为原始坐标
static int32_t processDesktopOriginY(TpWidget *dealWidget, int32_t processY)
{
    if (!dealWidget)
        return processY;

    // 检查当前对象类型
    Tp::ItpObjectType objType = dealWidget->objectType();
    bool isTopOrFloat = (objType == Tp::TP_TOP_OBJECT || objType == Tp::TP_FLOAT_OBJECT);

    // 检查父对象类型
    bool parentIsTop = false;
    if (!isTopOrFloat)
    {
        if (TpWidget *parentPtr = dynamic_cast<TpWidget *>(dealWidget->parent()))
        {
            parentIsTop = (parentPtr->objectType() == Tp::TP_TOP_OBJECT);
        }
    }

    // 合并条件并执行偏移计算
    if (isTopOrFloat || parentIsTop)
    {
        TpAppData *appData = static_cast<TpAppData *>(TpApp::Inst()->appObjectSet());
        if (!appData->isDesk && appData->desktopBarInfo_.topBarisVislble)
        {
            processY -= appData->desktopBarInfo_.topBarHeight;
        }
    }

    return processY;
}

// 根据桌面工具栏高度，处理窗体高度
static int32_t processDeskTopBarHeight(TpWidget *dealWidget, int32_t originHeight)
{
    if (!dealWidget)
        return originHeight;

    // 检查当前对象类型
    if (dealWidget->objectType() == Tp::TP_TOP_OBJECT)
    {
        TpAppData *appData = static_cast<TpAppData *>(TpApp::Inst()->appObjectSet());
        if (!appData->isDesk && appData->desktopBarInfo_.topBarisVislble)
        {
            originHeight -= appData->desktopBarInfo_.topBarHeight;
        }
    }

    return originHeight;
}
#endif

// 刷新缓存背景图
static void refreshCacheImage(TpObjectData *set)
{
    if (!set)
        return;

    if (set->reserveImage.isNull())
        return;

    if (set->logicalRect.width() == 0 || set->logicalRect.height() == 0)
        return;

    set->cacheImage = set->reserveImage.scaled(set->logicalRect.width(), set->logicalRect.height(), set->keepAspectRatio);

    // static int testIndex = 1;
    // set->cacheImage.save("/home/hawk/Public/TinyPiXOS/examples/TpGUI/test/cache-" + TpString::number(testIndex++) + ".png", TpImage::PNG_FMT);
    // set->reserveImage.save("/home/hawk/Public/TinyPiXOS/examples/TpGUI/test/origin" + TpString::number(testIndex++) + ".png", TpImage::PNG_FMT);
}

static void changeXY(TpWidget *thisPtr, TpObjectData *set, int32_t x, int32_t y)
{
    if (!set)
        return;

    // y = processDeskTopBarY(thisPtr, y);

    int32_t ox = set->logicalRect.x();
    int32_t oy = set->logicalRect.y();

    if (ox != x || oy != y)
    {
        set->logicalRect.setX(x);
        set->logicalRect.setY(y);

        TpPoint point = selfToScreenPoint(thisPtr, x, y);

        set->absoluteRect.setX(point.x());
        set->absoluteRect.setY(point.y());

        ItpObjectMoveSet input;
        input.object = thisPtr;
        input.nx = x;
        input.ny = y;
        TpMoveEvent event;
        bool ret = event.construct(&input);

        if (ret)
        {
            thisPtr->onMoveEvent(&event);
        }
    }

    if (set->parent)
    {
        thisPtr->broadSetTop();
    }
}

static void changeWidth(TpWidget *thisPtr, TpObjectData *set, const uint32_t &w)
{
    if (!set)
        return;

    uint32_t ow = set->logicalRect.width();

    uint32_t setW = w;

    if (setW > set->maximumWidth)
        setW = set->maximumWidth;
    else if (setW < set->minimumWidth)
        setW = set->minimumWidth;
    else
    {
    }

    if (ow != setW)
    {
        set->logicalRect.setWidth(setW);
        set->absoluteRect.setWidth(setW);

        ItpObjectResizeSet input;
        input.object = thisPtr;
        input.nw = setW;
        input.nh = set->logicalRect.height();
        input.question = TpResizeEvent::TP_NORMAL_CHANGE;
        TpResizeEvent event;
        bool ret = event.construct(&input);

        if (ret)
        {
            refreshCacheImage(set);

            IssueObjEvent(thisPtr, event, onResizeEvent, true);
        }
    }

    if (set->parent)
    {
        thisPtr->broadSetTop();
    }
}

static void changeHeight(TpWidget *thisPtr, TpObjectData *set, const uint32_t &h)
{
    if (!set)
        return;

    uint32_t oh = set->logicalRect.height();

    uint32_t setH = h;

    if (setH > set->maximumHeight)
        setH = set->maximumHeight;
    else if (setH < set->minimumHeight)
        setH = set->minimumHeight;
    else
    {
    }

    if (oh != setH)
    {
        set->logicalRect.setHeight(setH);
        set->absoluteRect.setHeight(setH);

        ItpObjectResizeSet input;
        input.object = thisPtr;
        input.nw = set->logicalRect.width();
        input.nh = setH;
        input.question = TpResizeEvent::TP_NORMAL_CHANGE;
        TpResizeEvent event;
        bool ret = event.construct(&input);

        // std::cout << "Change Height " << setH << std::endl;

        if (ret)
        {
            refreshCacheImage(set);

            IssueObjEvent(thisPtr, event, onResizeEvent, true);
        }
    }

    if (set->parent)
    {
        thisPtr->broadSetTop();
    }
}

#endif