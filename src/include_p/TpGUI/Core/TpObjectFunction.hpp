#ifndef TP_OBJECT_FUNCTION_HPP
#define TP_OBJECT_FUNCTION_HPP

#include "TpObject.h"
#include "TpWidget.h"
#include "TpPainter.h"
#include "thorVG/thorvg.h"

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

static inline void broadObjectSetTop(TpObject *object, TpObject *top) // clear topobject
{
    TpObjectData *set = static_cast<TpObjectData *>(object->objectSets());
    if (!set)
        return;

    set->top = top;

    if (set->top)
    {
        TpObject *parent = object->parent();

        if (parent)
        {
            TpObjectData *parent_set = (TpObjectData *)parent->objectSets();
            set->absoluteRect.setX(set->logicalRect.x() + parent_set->absoluteRect.x());
            set->absoluteRect.setY(set->logicalRect.y() + parent_set->absoluteRect.y());
        }

        TpWidget *parentWidget = static_cast<TpWidget *>(set->top);
        if (parentWidget)
        {
            set->offsetX = parentWidget->toScreen().x();
            set->offsetY = parentWidget->toScreen().y();

            if (set->objectList.size())
            {
                std::list<TpObject *>::iterator iter = set->objectList.begin();

                for (; iter != set->objectList.end(); iter++)
                {
                    broadObjectSetTop(*iter, set->top);
                }
            }
        }
    }
}

static inline bool addObject(TpObjectData *set, TpObject *object, TpObject *parent)
{
    if (object == nullptr ||
        object->objectType() == Tp::TP_TOP_OBJECT ||
        object->objectType() == Tp::TP_FLOAT_OBJECT)
    {
        return false;
    }

    set->gMutex.lock();
    TpObjectData *child_set = (TpObjectData *)object->objectSets();
    child_set->parent = parent;
    set->objectList.push_back(object);
    broadObjectSetTop(object, object->topObject());
    set->gMutex.unlock();

    return true;
}

static inline bool delObject(TpObjectData *set, TpObject *object)
{
    if (object == nullptr)
    {
        return false;
    }

    set->gMutex.lock();

    TpObjectData *child_set = (TpObjectData *)object->objectSets();
    set->objectList.remove(object);

    set->tmp.deleteObject(object);

    child_set->parent = nullptr;
    broadObjectSetTop(object, nullptr);
    set->gMutex.unlock();

    return true;
}

static inline TpObject *findObject(TpObjectData *set, int32_t id)
{
    TpObject *object = nullptr;

    if (id < 0)
    {
        return nullptr;
    }

    set->gMutex.lock();

    std::list<TpObject *> list = set->objectList;
    std::list<TpObject *>::iterator iter = list.begin();

    for (; iter != list.end(); iter++)
    {
        if ((*iter)->objectID() == id)
        {
            object = *iter;
            break;
        }

        TpObjectData *child_set = (TpObjectData *)(*iter)->objectSets();
        object = findObject(child_set, id);

        if (object)
        {
            break;
        }
    }

    set->gMutex.unlock();

    return object;
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

static inline TpWidget *findObject(TpObjectData *set, int32_t x, int32_t y)
{
    TpWidget *object = nullptr;

    set->gMutex.lock();

    std::list<TpObject *> list = set->objectList;
    std::list<TpObject *>::iterator iter = list.begin();

    for (; iter != list.end(); iter++)
    {
        TpWidget *childWidgetPtr = dynamic_cast<TpWidget *>(*iter);
        if (!childWidgetPtr)
            continue;

        if (!childWidgetPtr->visible())
            continue;

        // childWidgetPtr->TestFunction();

        TpObjectData *child_set = (TpObjectData *)childWidgetPtr->objectSets();
        bool ret = false;

        TpRect absRect(child_set->absoluteRect);
        ret = absRect.contains(x, y);

        if (ret)
        {
            object = childWidgetPtr;
        }
    }

    if (object)
    {
        TpWidget *result = nullptr;
        TpObjectData *child_set = (TpObjectData *)object->objectSets();

        if (child_set)
        {
            result = findObject(child_set, x, y);
        }

        if (result)
        {
            object = result;
        }
    }

    set->gMutex.unlock();

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

    auto canvasPair = obj->canvasPtr();
    tvg::SwCanvas *childCanvas = (tvg::SwCanvas *)canvasPair.first;
    tvg::Scene *childScene = (tvg::Scene *)canvasPair.second;

    childPainter->addScene(childCanvas, childScene);

    bool ret = obj->onPaintEvent(&event);

    // 清除所有现有效果
    childScene->push(tvg::SceneEffect::ClearAll);
    if (obj->enableBlur())
    {
        TpGraphicsBlurEffect blurEffectObj = obj->graphicsEffect();
        childScene->push(tvg::SceneEffect::GaussianBlur, blurEffectObj.blurRadius(), (int32_t)blurEffectObj.direction(), (int32_t)blurEffectObj.border(), blurEffectObj.quality());
    }

    // 控件不可用，绘制遮罩层
    paintEnabledBox(obj, event.painter());

    // 绘制完成刷新绘制
    childPainter->sync(obj);

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

        if (child->alpha() == 0)
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
