#ifndef __TP_OBJECT_PRIVATE_H
#define __TP_OBJECT_PRIVATE_H

#include "TpAutoObject.h"
#include <TpRect.h>
#include <TpPoint.h>
#include <tinyPiXUtils.h>
#include <TpHash.h>
#include <TpString.h>
#include <TpVariant.h>
#include "TpSignalSlot.h"

#include <mutex>
#include <iostream>

struct TpObjectData
{
    // 事件过滤器对象
    TpObject *filterObject = nullptr;

    // 所有子节点
    TpList<TpObject *> objectList;

    // 父指针和顶级指针
    TpObject *parent = nullptr;
    TpObject *top = nullptr;

    int32_t objectID;

    IPiWFApiAgent *agent = nullptr;
    std::mutex gMutex;

    // TpObjectStack *objectStack = nullptr;

    // 对象属性信息
    TpHash<TpString, TpVariant> objPropertyMap;

    // 缓存有多少发送者信号绑定了自己
    std::unordered_map<void *, std::vector<std::function<void()>>> slotConnections_;
    std::mutex slotConnectMutex_;

    TpObjectData()
    {
    }
    virtual ~TpObjectData()
    {
    }
};

static void disconnectAllSignal(TpObjectData *set)
{
    // 断开所有槽函数连接
    std::lock_guard<std::mutex> lock(set->slotConnectMutex_);
    for (auto &pair : set->slotConnections_)
    {
        for (auto &disconnector : pair.second)
        {
            disconnector();
        }
    }
    set->slotConnections_.clear();
}

static inline bool addObject(TpObjectData *set, TpObject *object, TpObject *parent)
{
    if (object == nullptr ||
        object->objectType() == Tp::TP_FIXSCREEN_OBJECT ||
        object->objectType() == Tp::TP_MAIN_WINDOW_OBJECT ||
        object->objectType() == Tp::TP_FLOAT_OBJECT)
    {
        return false;
    }

    set->gMutex.lock();
    TpObjectData *childSet = (TpObjectData *)object->objectSets();
    childSet->parent = parent;
    set->objectList.push_back(object);
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

    TpObjectData *childSet = (TpObjectData *)object->objectSets();
    set->objectList.remove(object);

    childSet->parent = nullptr;
    childSet->top = nullptr;
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

#endif