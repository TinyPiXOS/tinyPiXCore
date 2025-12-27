#ifndef TP_OBJECT_FUNCTION_HPP
#define TP_OBJECT_FUNCTION_HPP

#include "TpObject.h"
#include "TpWidget.h"
#include "TpPainter.h"
#include "thorVG/thorvg.h"
#include "TpObject_p.h"
#include "TpWidget_p.h"

// clear topobject
static inline void broadObjectSetTop(TpObject *object, TpObject *top)
{
    TpObjectData *set = static_cast<TpObjectData *>(object->objectSets());
    if (!set)
        return;

    set->top = top;
    if (!set->top)
        return;

    // 如果对象不是UI对象，不继续处理
    TpWidget *tmpObjWidget = dynamic_cast<TpWidget *>(object);
    if (!tmpObjWidget)
        return;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(set);

    TpWidget *parentWidget = dynamic_cast<TpWidget *>(object->parent());
    if (parentWidget)
    {
        TpWidgetData *parentWidgetData = static_cast<TpWidgetData *>(parentWidget->objectSets());

        widgetData->absoluteRect.setX(widgetData->logicalRect.x() + parentWidgetData->absoluteRect.x());
        widgetData->absoluteRect.setY(widgetData->logicalRect.y() + parentWidgetData->absoluteRect.y());
    }

    TpWidget *topWidget = static_cast<TpWidget *>(widgetData->top);
    if (topWidget)
    {
        widgetData->offsetX = topWidget->toScreen().x();
        widgetData->offsetY = topWidget->toScreen().y();

        for (const auto &childObj : widgetData->objectList)
        {
            broadObjectSetTop(childObj, widgetData->top);
        }
    }
}

static inline TpPoint selfToObjectPoint(TpObject *object, int32_t x, int32_t y)
{
    TpPoint point = {x, y};

    TpObjectData *set = static_cast<TpObjectData *>(object->objectSets());
    if (!set)
        return point;

    TpWidget *parentWidget = static_cast<TpWidget *>(set->parent);
    if (!parentWidget)
        return point;

    TpRect rect = parentWidget->toScreen();
    point.setX(point.x() - rect.x());
    point.setY(point.y() - rect.y());

    return point;
}

static inline TpWidget *findObject(TpWidgetData *widgetData, int32_t x, int32_t y)
{
    TpWidget *object = nullptr;

    widgetData->gMutex.lock();

    for (const auto &childObj : widgetData->objectList)
    {
        TpWidget *childWidgetPtr = dynamic_cast<TpWidget *>(childObj);
        if (!childWidgetPtr)
            continue;

        if (!childWidgetPtr->visible())
            continue;

        TpWidgetData *childWidgetData = static_cast<TpWidgetData *>(childWidgetPtr->objectSets());
        bool ret = false;

        // TODO dialog被强制move偏移状态栏后，子组件未及时响应坐标更新，导致第一次显示点击坐标错误问题

        TpRect absRect(childWidgetData->absoluteRect);
        ret = absRect.contains(x, y);

        if (ret)
        {
            object = childWidgetPtr;
        }
    }

    if (object)
    {
        TpWidget *result = nullptr;

        TpWidgetData *objData = (TpWidgetData *)object->objectSets();
        if (objData)
        {
            result = findObject(objData, x, y);
        }

        if (result)
        {
            object = result;
        }
    }

    widgetData->gMutex.unlock();

    return object;
}

static void paintEnabledBox(TpWidget *child, TpPainter *paintCanvas)
{
    if (child->enabled())
        return;

    if (child->parent())
    {
        TpWidget *parentWidget = dynamic_cast<TpWidget *>(child->parent());
        if (parentWidget && parentWidget->enabled())
        {
            paintCanvas->setPen(_RGBA(192, 192, 192, 80));
            paintCanvas->setBrush(TpBrush(_RGBA(192, 192, 192, 80)));
            paintCanvas->drawRect(0, 0, child->width(), child->height(), child->roundCorners());
        }
    }
    else
    {
        paintCanvas->setPen(_RGBA(192, 192, 192, 80));
        paintCanvas->setBrush(TpBrush(_RGBA(192, 192, 192, 80)));
        paintCanvas->drawRect(0, 0, child->width(), child->height(), child->roundCorners());
    }
}

// 先声明，因为 childPaint 和 drawWidget 互相调用了
static inline void childPaint(TpObjectData *set, TpPaintEvent *events);
static void drawWidget(TpPaintEventInput &input, TpWidget *obj)
{
    TpPaintEvent event;
    event.construct(&input);

    // 刷新前清除scene
    TpPainter *childPainter = event.painter();

    TpScreen *topScreen = dynamic_cast<TpScreen *>(obj->topObject());
    tvg::SwCanvas *topCanvas = (tvg::SwCanvas *)topScreen->canvasPtr();

    tvg::Scene *childScene = (tvg::Scene *)obj->scenePtr();
    childPainter->setScene(topCanvas, childScene);

    // 重新放入scene到父组件的scene
    childScene->remove();
    TpWidget *parentWidget = dynamic_cast<TpWidget *>(obj->parent());
    if (parentWidget)
    {
        TpWidgetData *parentWidgetData = static_cast<TpWidgetData *>(parentWidget->objectSets());
        TpWidgetData *widgetData = static_cast<TpWidgetData *>(obj->objectSets());
        parentWidgetData->tvgScene->push(widgetData->tvgScene);
    }

    bool ret = obj->onPaintEvent(&event);

    // 叠加透明度
    childScene->opacity(255 * obj->windowOpacity());

    // 清除所有现有效果
    childScene->push(tvg::SceneEffect::ClearAll);
    if (obj->enableGraphicsEffect())
    {
        TpGraphicsBlurEffect blurEffectObj = obj->graphicsEffect();
        childScene->push(tvg::SceneEffect::GaussianBlur, blurEffectObj.blurRadius(), (int32_t)blurEffectObj.direction(), (int32_t)blurEffectObj.border(), blurEffectObj.quality());
    }

    // 控件不可用，绘制遮罩层
    paintEnabledBox(obj, event.painter());

    if (ret)
    {
        TpObjectData *childSet = (TpObjectData *)obj->objectSets();
        childPaint(childSet, &event);
    }
}

static inline void childPaint(TpObjectData *set, TpPaintEvent *events)
{
    if (!set)
        return;

    std::list<TpObject *>::iterator iter = set->objectList.begin();

    for (; iter != set->objectList.end(); iter++)
    {
        TpWidget *child = dynamic_cast<TpWidget *>(*iter);
        if (!child)
            continue;

        TpRect updateRect = events->updateRect();
        TpRect childRect = child->toScreen();

        // std::cout << "updateRect区域： " << updateRect.x() << " , " << updateRect.y()
        //           << " , " << updateRect.width() << " , " << updateRect.height() << std::endl;
        // std::cout << "childRect: " << childRect.x() << " , " << childRect.y()
        //           << " , " << childRect.width() << " , " << childRect.height() << std::endl;

        if (!updateRect.intersect(childRect))
            continue;

        if (!child->visible())
            continue;

        if (tpFuzzyCompare(child->windowOpacity(), 0))
            continue;

        TpObjectData *childSet = (TpObjectData *)child->objectSets();
        TpPaintEventInput input;
        input.object = child;
        input.updateRect = events->updateRect();

        drawWidget(input, child);
    }
}

#endif
