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
#include "TpGlobal.h"
#include "TpDefaultCss.h"
#include "TpScreen.h"
#include "TpVirtualKeyboard.h"
#include "TpImage.h"
#include "TpBrush.h"
#include "TpWidget.h"

#include "thorVG/thorvg.h"
#include "TpSurface.h"
#include "tinyPiXUtils.h"
#include "tinyPiXApi.h"
#include "TpObject_p.h"

#include <png.h>
#include <unordered_map>
#include <mutex>
#include <thread>

struct TpWidgetData : TpObjectData
{
    // 对象类型；用于区分当前应用是否是桌面
    TpString objectType = "";

    // 鼠标按下的对象，用于判断拖拽、等事件
    // 记录鼠标按下时的对象，最后鼠标无论在哪释放，都触发按下对象的release
    TpWidget *mousePressObject = nullptr;

    // 是否显示；是否可用
    bool visible = true;
    bool enable = true;

    std::mutex layoutMutex;
    TpLayout *layout = nullptr;

    // XY偏移量
    int32_t offsetX;
    int32_t offsetY;

    /// @brief 绝对坐标
    TpRect absoluteRect;
    /// @brief 逻辑坐标
    TpRect logicalRect;

    // 物理屏幕尺寸
    TpSize displaySize;

    // 窗口最小宽高
    uint32_t minimumWidth = 0;
    uint32_t minimumHeight = 0;

    // 窗口最大宽高
    uint32_t maximumWidth = WIDGET_MAX_WIDTH;
    uint32_t maximumHeight = WIDGET_MAX_HEIGHT;

    // 背景图片
    bool enableImage;
    TpImage reserveImage;
    TpImage cacheImage;
    bool keepAspectRatio = true;

    // 背景颜色
    bool enableColor = true;
    uint32_t backColor;
    TpBrush backBrush;

    // 边框颜色
    bool enableBorderColor = false;
    uint32_t borderColor;
    TpBrush borderBrush;

    // 是否可选中；选中状态
    bool checkable = false;
    bool isChecked = false;

    // 是否启用背景模糊，模糊半径 px
    bool enableBlur = false;
    TpGraphicsBlurEffect blurEffect;

    bool isHover = false;
    bool isPress = false;
    TpPoint pressPoint;

    // 缓存状态信息
    ItpTempDef tmp;

    // 圆角值，单位px
    uint32_t round = 0;

    // 窗口不透明度乘数，[0,1]，1=完全不透明
    float windowOpacity = 1.0;

    // 组件抓图
    TpImage grapImage;

    // CSS数据
    tpShared<TpCssData> enabledCssData;
    tpShared<TpCssData> pressCssData;
    tpShared<TpCssData> hoverCssData;
    tpShared<TpCssData> checkedCssData;
    tpShared<TpCssData> disabledCssData;

    // 绘制画布和场景
    tvg::Scene *tvgScene = nullptr;
    TpRect m_lastClipRect;
    bool m_clipRectDirty{true};

    TpWidgetData()
    {
        enabledCssData = nullptr;
        pressCssData = nullptr;
        hoverCssData = nullptr;
        checkedCssData = nullptr;
        disabledCssData = nullptr;
    }

    virtual ~TpWidgetData()
    {
    }
};

// 刷新缓存背景图
static void refreshCacheImage(TpWidgetData *widgetData)
{
    if (!widgetData)
        return;

    if (widgetData->reserveImage.isNull())
        return;

    if (widgetData->logicalRect.width() == 0 || widgetData->logicalRect.height() == 0)
        return;

    widgetData->cacheImage = widgetData->reserveImage.scaled(widgetData->logicalRect.width(),
                                                             widgetData->logicalRect.height(), widgetData->keepAspectRatio);

    // static int testIndex = 1;
    // widgetData->cacheImage.save("/home/hawk/Public/TinyPiXOS/examples/TpGUI/test/cache-" + TpString::number(testIndex++) + ".png", TpImage::PNG_FMT);
    // widgetData->reserveImage.save("/home/hawk/Public/TinyPiXOS/examples/TpGUI/test/origin" + TpString::number(testIndex++) + ".png", TpImage::PNG_FMT);
}

static inline TpPoint selfToScreenPoint(TpObject *object, int32_t x, int32_t y)
{
    TpPoint point(x, y);

    TpObjectData *set = static_cast<TpObjectData *>(object->objectSets());
    if (!set)
        return point;

    TpWidget *parentWidget = static_cast<TpWidget *>(set->parent);
    if (!parentWidget)
        return point;

    TpRect rect = parentWidget->toScreen();
    point.setX(point.x() + rect.x());
    point.setY(point.y() + rect.y());

    return point;
}

static void changeXY(TpWidget *thisPtr, TpWidgetData *widgetData, int32_t x, int32_t y)
{
    if (!thisPtr)
        return;

    // TpMainWindow不可被移动坐标
    if (thisPtr->pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    if (!widgetData)
        return;

    int32_t ox = widgetData->logicalRect.x();
    int32_t oy = widgetData->logicalRect.y();

    if (ox != x || oy != y)
    {
        widgetData->logicalRect.setX(x);
        widgetData->logicalRect.setY(y);

        TpPoint point = selfToScreenPoint(thisPtr, x, y);

        widgetData->absoluteRect.setX(point.x());
        widgetData->absoluteRect.setY(point.y());

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

    if (widgetData->parent)
    {
        thisPtr->broadSetTop();
    }
}

static void changeWidth(TpWidget *thisPtr, TpWidgetData *widgetData, const uint32_t &w)
{
    if (!thisPtr)
        return;

    // TpMainWindow不可被修改尺寸
    if (thisPtr->pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    if (!widgetData)
        return;

    uint32_t ow = widgetData->logicalRect.width();

    uint32_t setW = w;

    if (setW > widgetData->maximumWidth)
        setW = widgetData->maximumWidth;
    else if (setW < widgetData->minimumWidth)
        setW = widgetData->minimumWidth;
    else
    {
    }

    if (ow != setW)
    {
        widgetData->logicalRect.setWidth(setW);
        widgetData->absoluteRect.setWidth(setW);

        ItpObjectResizeSet input;
        input.object = thisPtr;
        input.nw = setW;
        input.nh = widgetData->logicalRect.height();
        input.question = TpResizeEvent::TP_NORMAL_CHANGE;
        TpResizeEvent event;
        bool ret = event.construct(&input);

        if (ret)
        {
            refreshCacheImage(widgetData);
            IssueObjEvent(thisPtr, event, onResizeEvent, true);
        }
    }

    if (widgetData->parent)
    {
        thisPtr->broadSetTop();
    }
}

static void changeHeight(TpWidget *thisPtr, TpWidgetData *widgetData, const uint32_t &h)
{
    if (!thisPtr)
        return;

    // TpMainWindow不可被修改尺寸
    if (thisPtr->pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    if (!widgetData)
        return;

    uint32_t oh = widgetData->logicalRect.height();

    uint32_t setH = h;

    if (setH > widgetData->maximumHeight)
        setH = widgetData->maximumHeight;
    else if (setH < widgetData->minimumHeight)
        setH = widgetData->minimumHeight;
    else
    {
    }

    if (oh != setH)
    {
        widgetData->logicalRect.setHeight(setH);
        widgetData->absoluteRect.setHeight(setH);

        ItpObjectResizeSet input;
        input.object = thisPtr;
        input.nw = widgetData->logicalRect.width();
        input.nh = setH;
        input.question = TpResizeEvent::TP_NORMAL_CHANGE;
        TpResizeEvent event;
        bool ret = event.construct(&input);

        // std::cout << "Change Height " << setH << std::endl;

        if (ret)
        {
            refreshCacheImage(widgetData);
            IssueObjEvent(thisPtr, event, onResizeEvent, true);
        }
    }

    if (widgetData->parent)
    {
        thisPtr->broadSetTop();
    }
}

#if 0
// 组件刷新限制区域，所有子节点都要刷新
static void refreshSceneClipRect(TpWidget *widget, TpWidgetData *widgetData);
static void refreshChildSceneClipRect(TpWidget *widget, TpWidgetData *widgetData)
{
    // 所有子节点也要重新设置裁剪区域
    for (const auto &childObj : widgetData->objectList)
    {
        TpWidget *childWidget = dynamic_cast<TpWidget *>(childObj);
        if (!childWidget)
            continue;
        TpWidgetData *childData = static_cast<TpWidgetData *>(childWidget->objectSets());
        refreshSceneClipRect(childWidget, childData);
    }
};

// 刷新组件限制区域
static void refreshSceneClipRect(TpWidget *widget, TpWidgetData *widgetData)
{
    // return;

    // 所有子节点也要重新设置裁剪区域
    refreshChildSceneClipRect(widget, widgetData);

    if (widget->pluginType().compare(TO_STRING(TpMediaTileButton)) == 0)
    {
        int a = 0;
    }

    // 重设限制区域
    if (!widget || !widgetData->tvgScene)
        return;

    Tp::TpObjectType type = widget->objectType();
    const TpRect objectAbsRect = widget->toScreen();
    const int32_t objWidth = widget->width();
    const int32_t objHeight = widget->height();
    const int32_t offsetXVal = widget->offsetX();
    const int32_t offsetYVal = widget->offsetY();

    int32_t offsetX = 0;
    int32_t offsetY = 0;
    if (type == Tp::TP_FLOAT_OBJECT || type == Tp::TP_MAIN_WINDOW_OBJECT || type == Tp::TP_FIXSCREEN_OBJECT)
    {
        offsetX = 0;
        offsetY = 0;
    }
    else
    {
        offsetX = objectAbsRect.x() - offsetXVal;
        offsetY = objectAbsRect.y() - offsetYVal;
    }

    // 限制绘制区域;如果父窗口比自己大，则使用自己的尺寸，如果父窗口比自己小，则使用父窗口的
    TpRect clipRect = objectAbsRect;

    TpWidget *inputParentWidget = dynamic_cast<TpWidget *>(widget->parent());
    if (inputParentWidget)
    {
        while (inputParentWidget)
        {
            TpRect inputParentRect = inputParentWidget->toScreen();

            // std::cout << "inputParentRect: " << inputParentRect.x() << " , " << inputParentRect.y()
            //           << " , " << inputParentRect.width() << " , " << inputParentRect.height() << std::endl;
            // std::cout << "clipRect: " << clipRect.x() << " , " << clipRect.y()
            //           << " , " << clipRect.width() << " , " << clipRect.height() << std::endl;

            clipRect.setX(TP_MAX(clipRect.x(), inputParentRect.x()));
            clipRect.setY(TP_MAX(clipRect.y(), inputParentRect.y()));

            int32_t tempWidth = TP_MIN(clipRect.x() + clipRect.width(), inputParentRect.x() + inputParentRect.width());
            int32_t tempHeight = TP_MIN(clipRect.y() + clipRect.height(), inputParentRect.y() + inputParentRect.height());

            // std::cout << "tempWidth: " << tempWidth << " , tempHeight: " << tempHeight << std::endl;

            clipRect.setWidth(tempWidth - clipRect.x());
            clipRect.setHeight(tempHeight - clipRect.y());

            // std::cout << "After clipRect: " << clipRect.x() << " , " << clipRect.y()
            //   << " , " << clipRect.width() << " , " << clipRect.height() << std::endl;

            inputParentWidget = dynamic_cast<TpWidget *>(inputParentWidget->parent());
        }
    }
    else
    {
        clipRect.setX(offsetX);
        clipRect.setY(offsetY);
        clipRect.setWidth(objWidth);
        clipRect.setHeight(objHeight);
    }

    TpObject *top = widget->topObject();
    if (top && (top != widget) && (top->objectType() == Tp::TP_FLOAT_OBJECT || top->objectType() == Tp::TP_MAIN_WINDOW_OBJECT))
    {
        clipRect.setX(clipRect.x() - offsetXVal);
        clipRect.setY(clipRect.y() - offsetYVal);
    }

    // 为每个Scene创建一个矩形裁剪区域
    auto clipper = tvg::Shape::gen();
    // Scene的边界
    clipper->appendRect(clipRect.x(), clipRect.y(),
                        clipRect.width(), clipRect.height());

    // std::cout << "Widget Clip Rect: " << widget->pluginType() << " , " << clipRect.x() << " , " << clipRect.y()
    //   << " , " << clipRect.width() << " , " << clipRect.height() << std::endl;

    // 将裁剪器应用到Scene
    // 获取并释放旧的裁剪器
    if (auto oldClipper = widgetData->tvgScene->clip())
    {
        tvg::Paint::rel(oldClipper);
    }
    auto result = widgetData->tvgScene->clip(clipper);
    if (result != tvg::Result::Success)
    {
        tvg::Paint::rel(clipper);
    }
};

#endif

static void setChildVisible(TpWidgetData *widgetData, bool visible);
static void setLayoutVisible(TpObject *layout, bool visible)
{
    TpLayout *childLayout = dynamic_cast<TpLayout *>(layout);
    if (!childLayout)
        return;

    TpList<TpObject *> layoutChildList = childLayout->objectList();
    for (const auto &layoutChild : layoutChildList)
    {
        TpWidget *childWidget = dynamic_cast<TpWidget *>(layoutChild);
        if (!childWidget)
        {
            setLayoutVisible(layoutChild, visible);
            continue;
        }

        childWidget->setVisible(visible);

        TpWidgetData *childWidgetData = static_cast<TpWidgetData *>(childWidget->objectSets());
        if (childWidgetData->tvgScene)
        {
            childWidgetData->tvgScene->visible(visible);
        }
        setChildVisible(childWidgetData, visible);
    }
};

static void setChildVisible(TpWidgetData *widgetData, bool visible)
{
    if (widgetData->tvgScene)
    {
        widgetData->tvgScene->visible(visible);
    }

    for (const auto &childObj : widgetData->objectList)
    {
        TpWidget *childWidget = dynamic_cast<TpWidget *>(childObj);
        if (!childWidget)
        {
            setLayoutVisible(childObj, visible);
            continue;
        }

        childWidget->setVisible(visible);

        TpWidgetData *childWidgetData = static_cast<TpWidgetData *>(childWidget->objectSets());
        if (childWidgetData->tvgScene)
        {
            childWidgetData->tvgScene->visible(visible);
        }
        setChildVisible(childWidgetData, visible);
    }
}

#endif