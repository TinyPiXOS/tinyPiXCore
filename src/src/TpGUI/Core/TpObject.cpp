#include "TpObject.h"
#include "TpAutoObject.h"
#include "TpWidget.h"
#include "TpEvent.h"
#include "TpApp.h"
#include "TpPainter.h"
#include "TpLayout.h"
#include "TpDef.h"
#include "TpObjectStack.h"
#include <TpSurface.h>
#include <TpColors.h>
#include <TpRect.h>
#include <TpPoint.h>
#include <tinyPiXUtils.h>
#include <TpHash.h>
#include <TpString.h>
#include <TpVariant.h>
#include "Core/TpObjectFunction.hpp"
#include "TpSignalSlot.h"

#include <mutex>
#include <iostream>

void disconnectAllSignal(TpObjectData *set)
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

TpObject::TpObject(TpObject *parent)
{
    TpObjectData *set = new TpObjectData();

    if (!set)
        return;

    set->offsetX = 0;
    set->offsetY = 0;

    set->backColor = _RGB(248, 248, 248);
    // set->backColor = TpApp::Inst()->appConfigSet()->bkcolor;
    set->objectID = TpAutoObject::Inst()->selfCounterIncrease();

    set->top = nullptr;

    set->agent = nullptr;

    set->enableColor = true;
    // set->enableColor = TpApp::Inst()->appConfigSet()->enable;
    set->enableImage = true;

    set->windowOpacity = 1.0;

    set->layout = nullptr;

    set->visible = false;

    memset(set->text, 0, OBJECT_MAX_TEXT_LENGTH);

    data_ = set;
}

TpObject::~TpObject()
{
    bool ret = TpApp::Inst()->isExistObject(this, true);

    if (!ret)
        return;

    TpObjectData *set = static_cast<TpObjectData *>(data_);
    if (!set)
        return;

    disconnectAllSignal(set);

    set->gMutex.lock();

    if (set->parent)
    {
        TpObjectData *parent_set = (TpObjectData *)set->parent->objectSets();
        delObject(parent_set, set->parent);
    }

    set->objectList.clear();
    set->gMutex.unlock();

    delete set;
}

void TpObject::setProperty(const TpString &_name, const TpVariant &_value)
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);

    set->objPropertyMap[_name] = _value;
}

TpVariant TpObject::property(const TpString &_name)
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);

    if (set->objPropertyMap.contains(_name))
        return set->objPropertyMap[_name];
    return TpVariant();
}

void TpObject::installEventFilter(TpObject *filterObj)
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);
    if (!set)
        return;

    set->filterObject = filterObj;
}

void TpObject::uninstallEventFilter()
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);
    if (!set)
        return;

    set->filterObject = nullptr;
}

TpObject *TpObject::eventFilterObject()
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);
    if (!set)
        return nullptr;

    return set->filterObject;
}

bool TpObject::eventFilter(TpObject *watched, TpEvent *event)
{
    return false;
}

void TpObject::broadSetTop()
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);

    if (set)
    {
        broadObjectSetTop(this, set->top);
    }
}

TpObject *TpObject::find(int32_t id)
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);
    TpObject *object = this;

    if (set)
    {
        if (id == set->objectID)
        {
            object == this;
        }
        else
        {
            object = findObject(set, id);
        }
    }

    return object;
}

void TpObject::deleteLater()
{
    // 立刻终止信号槽绑定
    TpObjectData *set = static_cast<TpObjectData *>(data_);
    if (!set)
        return;

    disconnectAllSignal(set);

    TpApp::Inst()->sendDelete(this);
}

int32_t TpObject::objectID()
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);
    int32_t id = TP_INVALIDATE_VALUE;

    if (set)
    {
        id = set->objectID;
    }

    return id;
}

TpList<TpObject *> &TpObject::objectList()
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);
    TpList<TpObject *> *objectList = nullptr;

    if (set)
    {
        objectList = &set->objectList;
    }

    return (*objectList);
}

void TpObject::setParent(TpObject *parent)
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);

    if (!set)
        return;

    if (set->parent == parent)
        return;

    // 如果已有父指针， 移除以前的父指针
    if (set->parent)
    {
        TpObjectData *parentSet = (TpObjectData *)set->parent->objectSets();
        delObject(parentSet, this);
    }

    if (parent)
    {
        TpObjectData *parentSet = (TpObjectData *)parent->objectSets();
        addObject(parentSet, this, parent);
    }
    else
    {
        broadObjectSetTop(this, nullptr);
    }

    set->parent = parent;
}

TpObject *TpObject::parent()
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);
    TpObject *parent = nullptr;

    if (set)
    {
        parent = set->parent;
    }

    return parent;
}

TpObject *TpObject::topObject()
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);
    TpObject *top = nullptr, *pParent = nullptr;

    if (!set)
        return top;

    Tp::TpObjectType type = objectType();

    if (type == Tp::TP_FIXSCREEN_OBJECT || type == Tp::TP_MAIN_WINDOW_OBJECT || type == Tp::TP_FLOAT_OBJECT)
    {
        set->top = this;
    }
    else
    {
        if (top == nullptr)
        {
            pParent = set->parent;
            while (pParent)
            {
                Tp::TpObjectType type = pParent->objectType();

                if (type == Tp::TP_FIXSCREEN_OBJECT || type == Tp::TP_MAIN_WINDOW_OBJECT || type == Tp::TP_FLOAT_OBJECT)
                {
                    set->top = pParent;
                    break;
                }

                pParent = pParent->parent();
            }
        }
    }

    top = set->top;

    return top;
}

ITpObjectData *TpObject::objectSets()
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);
    ITpObjectData *sets = nullptr;

    if (set)
    {
        sets = set;
    }

    return sets;
}

void *TpObject::operator new(size_t size)
{
    void *object = (void *)malloc(size);

    if (object == nullptr)
    {
        return NULL;
    }

    TpAutoObject *helper = TpAutoObject::Inst();

    if (helper)
    {
        helper->addObjectLife(object);
    }

    return object;
}

void *TpObject::operator new[](size_t size)
{
    void *object = (void *)malloc(size);

    if (object == nullptr)
    {
        return NULL;
    }

    TpAutoObject *helper = TpAutoObject::Inst();

    if (helper)
    {
        helper->addObjectLife(object);
    }

    return object;
}

void TpObject::operator delete(void *ptr)
{
    if (!ptr)
        return;

    TpAutoObject *helper = TpAutoObject::Inst();

    if (!helper)
        return;

    if (helper->isExist(ptr))
    {
        helper->removeObjectLife(ptr);
        free(ptr);
    }
}

void TpObject::operator delete[](void *ptr)
{
    if (!ptr)
        return;

    TpAutoObject *helper = TpAutoObject::Inst();

    if (!helper)
        return;

    if (helper->isExist(ptr))
    {
        helper->removeObjectLife(ptr);
        free(ptr);
    }
}

void TpObject::addConnection(void *signal, std::function<void()> disconnector)
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);

    std::lock_guard<std::mutex> lock(set->slotConnectMutex_);
    set->slotConnections_[signal].push_back(disconnector);
}
