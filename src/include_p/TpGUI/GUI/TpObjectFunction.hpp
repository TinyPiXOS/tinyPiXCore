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

    TpObject *parent = object->parent();
    TpWidget *parentWidget = dynamic_cast<TpWidget *>(parent);
    if (parent && parentWidget)
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

        if (widgetData->objectList.size())
        {
            std::list<TpObject *>::iterator iter = widgetData->objectList.begin();

            for (; iter != widgetData->objectList.end(); iter++)
            {
                broadObjectSetTop(*iter, widgetData->top);
            }
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

    std::list<TpObject *> list = widgetData->objectList;
    std::list<TpObject *>::iterator iter = list.begin();

    for (; iter != list.end(); iter++)
    {
        TpWidget *childWidgetPtr = dynamic_cast<TpWidget *>(*iter);
        if (!childWidgetPtr)
            continue;

        if (!childWidgetPtr->visible())
            continue;

        TpWidgetData *childWidgetData = static_cast<TpWidgetData *>(childWidgetPtr->objectSets());
        bool ret = false;

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
    // TODO 暂时屏蔽禁用绘制效果
    return;

    if (!child->enabled())
    {
        paintCanvas->setPen(_RGBA(192, 192, 192, 80));
        paintCanvas->setBrush(TpBrush(_RGBA(192, 192, 192, 80)));

        paintCanvas->drawRect(0, 0, child->width(), child->height(), child->roundCorners());
    }
}

// 先声明，因为 childPaint 和 drawWidget 互相调用了
static inline void childPaint(TpObjectData *set, TpPaintEvent *events);
static void drawWidget(ItpObjectPaintInput &input, TpWidget *obj)
{
    TpPaintEvent event;
    event.construct(&input);

    // 刷新前清除scene
    TpPainter *childPainter = event.painter();

    TpScreen *topScreen = dynamic_cast<TpScreen *>(obj->topObject());
    tvg::SwCanvas *topCanvas = (tvg::SwCanvas *)topScreen->canvasPtr();

    // auto canvasPair = obj->canvasPtr();
    // tvg::SwCanvas *childCanvas = (tvg::SwCanvas *)canvasPair.first;
    tvg::Scene *childScene = (tvg::Scene *)obj->scenePtr();

    // std::cout << "obj->pluginType() " << obj->pluginType() << std::endl;
    // std::list<tvg::Paint *> childSceneList = childScene->paints();
    // std::cout << "obj Scene Ptr : " << childScene << std::endl;
    // std::cout << "obj Scene ChildSize : " << childSceneList.size() << std::endl;

    childPainter->setScene(topCanvas, childScene);

    // std::list<tvg::Paint *> canvasChildList = topCanvas->paints();
    // std::cout << "topCanvas Child : " << canvasChildList.size() << std::endl;

    // tvg::Scene *topScene = (tvg::Scene *)canvasChildList.front();
    // std::cout << "topScene Scene Ptr : " << topScene << std::endl;
    // std::list<tvg::Paint *> canvasSceneList = topScene->paints();
    // std::cout << "top Scene ChildList : " << canvasSceneList.size() << std::endl;

    // 隐藏窗口将scene清空并从canvas移除
    if (!obj->visible())
    {
        childScene->remove();
        topCanvas->remove(childScene);
        return;
    }
    else
    {
        // 直接push，内部会判断不会重复添加
        topCanvas->push(childScene);
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

    // 绘制完成刷新绘制
    // childPainter->sync(obj);

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
        ItpObjectPaintInput input;
        input.object = child;
        input.updateRect = events->updateRect();
        input.surface = events->surface();

        drawWidget(input, child);
    }
}

#endif
