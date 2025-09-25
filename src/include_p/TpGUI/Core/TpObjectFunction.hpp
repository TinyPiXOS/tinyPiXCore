#ifndef TP_OBJECT_FUNCTION_HPP
#define TP_OBJECT_FUNCTION_HPP

#include "TpObject.h"
#include "TpChildWidget.h"
#include "TpPainter.h"
#include "thorVG/thorvg.h"

static inline TpPoint selfToScreenPoint(TpObject *object, int32_t x, int32_t y)
{
    TpPoint point(x, y);

    ItpObjectSet *set = static_cast<ItpObjectSet *>(object->objectSets());
    if (!set)
        return point;

    TpChildWidget *parentWidget = static_cast<TpChildWidget *>(set->parent);
    if (!parentWidget)
        return point;

    TpRect rect = parentWidget->toScreen();
    point.setX(point.x() + rect.x());
    point.setY(point.y() + rect.y());

    return point;
}

static inline void broadObjectSetTop(TpObject *object, TpObject *top) // clear topobject
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(object->objectSets());
    if (!set)
        return;

    set->top = top;

    if (set->top)
    {
        TpObject *parent = object->parent();

        if (parent)
        {
            ItpObjectSet *parent_set = (ItpObjectSet *)parent->objectSets();
            set->absoluteRect.setX(set->logicalRect.x() + parent_set->absoluteRect.x());
            set->absoluteRect.setY(set->logicalRect.y() + parent_set->absoluteRect.y());
        }

        TpChildWidget *parentWidget = static_cast<TpChildWidget *>(set->top);
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

static inline bool addObject(ItpObjectSet *set, TpObject *object, TpObject *parent)
{
    if (object == nullptr ||
        object->objectType() == Tp::TP_TOP_OBJECT ||
        object->objectType() == Tp::TP_FLOAT_OBJECT)
    {
        return false;
    }

    set->gMutex.lock();
    ItpObjectSet *child_set = (ItpObjectSet *)object->objectSets();
    child_set->parent = parent;
    set->objectList.push_back(object);
    broadObjectSetTop(object, object->topObject());
    set->gMutex.unlock();

    return true;
}

static inline bool delObject(ItpObjectSet *set, TpObject *object)
{
    if (object == nullptr)
    {
        return false;
    }

    set->gMutex.lock();

    ItpObjectSet *child_set = (ItpObjectSet *)object->objectSets();
    set->objectList.remove(object);

    set->tmp.deleteObject(object);

    child_set->parent = nullptr;
    broadObjectSetTop(object, nullptr);
    set->gMutex.unlock();

    return true;
}

static inline TpObject *findObject(ItpObjectSet *set, int32_t id)
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

        ItpObjectSet *child_set = (ItpObjectSet *)(*iter)->objectSets();
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

    ItpObjectSet *set = static_cast<ItpObjectSet *>(object->objectSets());
    if (!set)
        return point;

    TpChildWidget *parentWidget = static_cast<TpChildWidget *>(set->parent);
    if (!parentWidget)
        return point;

    TpRect rect = parentWidget->toScreen();
    point.setX(point.x() - rect.x());
    point.setY(point.y() - rect.y());

    return point;
}

static inline TpChildWidget *findObject(ItpObjectSet *set, int32_t x, int32_t y)
{
    TpChildWidget *object = nullptr;

    set->gMutex.lock();

    std::list<TpObject *> list = set->objectList;
    std::list<TpObject *>::iterator iter = list.begin();

    for (; iter != list.end(); iter++)
    {
        TpChildWidget *childWidgetPtr = dynamic_cast<TpChildWidget *>(*iter);
        if (!childWidgetPtr)
            continue;

        if (!childWidgetPtr->visible())
            continue;

        // childWidgetPtr->TestFunction();

        ItpObjectSet *child_set = (ItpObjectSet *)childWidgetPtr->objectSets();
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
        TpChildWidget *result = nullptr;
        ItpObjectSet *child_set = (ItpObjectSet *)object->objectSets();

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

static void paintEnabledBox(TpChildWidget *child, TpPainter *paintCanvas)
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

static inline void childPaint(ItpObjectSet *set, TpPaintEvent *events)
{
    if (!set)
        return;

    std::list<TpObject *>::iterator iter = set->objectList.begin();

    for (; iter != set->objectList.end(); iter++)
    {
        TpChildWidget *child = dynamic_cast<TpChildWidget *>(*iter);
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

        ItpObjectSet *childSet = (ItpObjectSet *)child->objectSets();
        ItpObjectPaintInput input;
        input.object = child;
        input.updateRect = events->updateRect();
        input.surface = events->surface();

        TpPaintEvent event;
        event.construct(&input);

        // 刷新前清除scene
        TpPainter *childPainter = event.painter();

        auto canvasPair = child->canvasPtr();
        tvg::SwCanvas *childCanvas = (tvg::SwCanvas *)canvasPair.first;
        tvg::Scene *childScene = (tvg::Scene *)canvasPair.second;

        childPainter->addScene(childCanvas, childScene);

        bool ret = child->onPaintEvent(&event);

        // 清除所有现有效果
        childScene->push(tvg::SceneEffect::ClearAll);
        if (child->enableBlur())
        {
            TpGraphicsBlurEffect blurEffectObj = child->graphicsEffect();
            childScene->push(tvg::SceneEffect::GaussianBlur, blurEffectObj.blurRadius(), (int32_t)blurEffectObj.direction(), (int32_t)blurEffectObj.border(), blurEffectObj.quality());
        }

        // 控件不可用，绘制遮罩层
        paintEnabledBox(child, event.painter());

        // 绘制完成刷新绘制
        childPainter->sync();

        if (ret)
        {
            childPaint(childSet, &event);
        }
    }
}

#endif
