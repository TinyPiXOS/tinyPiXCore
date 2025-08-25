#ifndef TP_OBJECT_FUNCTION_HPP
#define TP_OBJECT_FUNCTION_HPP

#include "tpObject.h"
#include "tpChildWidget.h"
#include "TpCanvas.h"
#include "thorVG/thorvg.h"

static inline ItpPoint selfToScreenPoint(tpObject *object, int32_t x, int32_t y)
{
    ItpPoint point(x, y);

    ItpObjectSet *set = static_cast<ItpObjectSet *>(object->objectSets());
    if (!set)
        return point;

    tpChildWidget *parentWidget = static_cast<tpChildWidget *>(set->parent);
    if (!parentWidget)
        return point;

    ItpRect rect = parentWidget->toScreen();
    point.x += rect.x;
    point.y += rect.y;

    return point;
}

static inline void broadObjectSetTop(tpObject *object, tpObject *top) // clear topobject
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(object->objectSets());
    if (!set)
        return;

    set->top = top;

    if (set->top)
    {
        tpObject *parent = object->parent();

        if (parent)
        {
            ItpObjectSet *parent_set = (ItpObjectSet *)parent->objectSets();
            set->absoluteRect.x = set->logicalRect.x + parent_set->absoluteRect.x;
            set->absoluteRect.y = set->logicalRect.y + parent_set->absoluteRect.y;
        }

        tpChildWidget *parentWidget = static_cast<tpChildWidget *>(set->top);
        if (parentWidget)
        {
            set->offsetX = parentWidget->toScreen().x;
            set->offsetY = parentWidget->toScreen().y;

            if (set->objectList.size())
            {
                std::list<tpObject *>::iterator iter = set->objectList.begin();

                for (; iter != set->objectList.end(); iter++)
                {
                    broadObjectSetTop(*iter, set->top);
                }
            }
        }
    }
}

static inline bool addObject(ItpObjectSet *set, tpObject *object, tpObject *parent)
{
    if (object == nullptr ||
        object->objectType() == TP_TOP_OBJECT ||
        object->objectType() == TP_FLOAT_OBJECT)
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

static inline bool delObject(ItpObjectSet *set, tpObject *object)
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

static inline tpObject *findObject(ItpObjectSet *set, int32_t id)
{
    tpObject *object = nullptr;

    if (id < 0)
    {
        return nullptr;
    }

    set->gMutex.lock();

    std::list<tpObject *> list = set->objectList;
    std::list<tpObject *>::iterator iter = list.begin();

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

static inline ItpPoint selfToObjectPoint(tpObject *object, int32_t x, int32_t y)
{
    ItpPoint point = {x, y};

    ItpObjectSet *set = static_cast<ItpObjectSet *>(object->objectSets());
    if (!set)
        return point;

    tpChildWidget *parentWidget = static_cast<tpChildWidget *>(set->parent);
    if (!parentWidget)
        return point;

    ItpRect rect = parentWidget->toScreen();
    point.x -= rect.x;
    point.y -= rect.y;

    return point;
}

static inline tpChildWidget *findObject(ItpObjectSet *set, int32_t x, int32_t y)
{
    tpChildWidget *object = nullptr;

    set->gMutex.lock();

    std::list<tpObject *> list = set->objectList;
    std::list<tpObject *>::iterator iter = list.begin();

    for (; iter != list.end(); iter++)
    {
        tpChildWidget *childWidgetPtr = dynamic_cast<tpChildWidget *>(*iter);
        if (!childWidgetPtr)
            continue;

        if (!childWidgetPtr->visible())
            continue;

        // childWidgetPtr->TestFunction();

        ItpObjectSet *child_set = (ItpObjectSet *)childWidgetPtr->objectSets();
        bool ret = false;

        tpRect absRect(child_set->absoluteRect);
        ret = absRect.in(x, y);

        if (ret)
        {
            object = childWidgetPtr;
        }
    }

    if (object)
    {
        tpChildWidget *result = nullptr;
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

static void paintEnabledBox(tpChildWidget *child, TpCanvas *paintCanvas)
{
    if (!child->enabled())
    {
        if (child->roundCorners() != 0)
            paintCanvas->roundedBox(0, 0, child->width(), child->height(), child->roundCorners(), _RGBA(192, 192, 192, 125));
        else
            paintCanvas->box(0, 0, child->width(), child->height(), _RGBA(192, 192, 192, 125));
    }
}

static inline void childPaint(ItpObjectSet *set, tpObjectPaintEvent *events)
{
    if (!set)
        return;

    std::list<tpObject *>::iterator iter = set->objectList.begin();

    for (; iter != set->objectList.end(); iter++)
    {
        tpChildWidget *child = dynamic_cast<tpChildWidget *>(*iter);
        if (!child)
            continue;

        ItpRect updateIRect = events->updateRect();
        tpRect updateRect(&updateIRect);

        ItpRect childRect = child->toScreen();

        if (!updateRect.intersect(&childRect))
        {
            continue;
        }

        if (!child->visible())
            continue;

        if (child->alpha() == 0)
            continue;

        ItpObjectSet *childSet = (ItpObjectSet *)child->objectSets();
        tpObjectPaintEvent event;
        ItpObjectPaintInput input;
        input.object = child;
        input.updateRect = events->updateRect();
        input.surface = events->surface();
        event.construct(&input);

        // 刷新前清除scene
        TpCanvas* childPainter = event.canvas();

        tvg::SwCanvas *childCanvas = (tvg::SwCanvas *)child->testCanvasPtr();
        tvg::Scene* childScene = (tvg::Scene*)child->testScenePtr();
        childScene->remove();
        childPainter->addScene(childCanvas, childScene);

        bool ret = child->onPaintEvent(&event);

        // 绘制完成刷新绘制
        childPainter->sync();

        if (ret)
        {
            childPaint(childSet, &event);
        }

        // 控件不可用，绘制遮罩层
        paintEnabledBox(child, event.canvas());
    }
}

#endif
