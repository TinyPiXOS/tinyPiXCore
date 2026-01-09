#include "TpObject.h"
#include <TpObject_p.h>
// #include "Core/TpObjectFunction.hpp"
#include <TpCoreEvent.h>

TpObject::TpObject(TpObject *parent) : data_(nullptr)
{
    TpObjectData *set = new TpObjectData();
    data_ = set;
    initTpData();
}

TpObject::~TpObject()
{
    if (TpCoreApp::Inst())
        TpCoreApp::Inst()->isExistObject(this, true);

    TpObjectData *objData = static_cast<TpObjectData *>(data_);
    if (!objData)
        return;

    disconnectAllSignal(objData);

    objData->gMutex.lock();

    // 从当前节点的父节点移除当前节点
    if (objData->parent)
    {
        TpObjectData *parentSet = (TpObjectData *)objData->parent->objectSets();
        delObject(parentSet, this);
    }

    // 删除当前节点的所有子节点
    for (auto &child : objData->objectList)
    {
        delete child;
        child = nullptr;
    }

    objData->objectList.clear();
    objData->gMutex.unlock();

    delete objData;
    objData = nullptr;
    data_ = nullptr;
}

void TpObject::setProperty(const TpString &_name, const TpVariant &_value)
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);

    set->objPropertyMap[_name] = _value;

    auto it = m_properties.find(_name);
    if (it != m_properties.end())
    {
        it->second.second(_value);
    }
}

TpVariant TpObject::property(const TpString &_name)
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);

    if (set->objPropertyMap.contains(_name))
    {
        return set->objPropertyMap[_name];
    }
    else
    {
        auto it = m_properties.find(_name);
        if (it != m_properties.end())
        {
            return it->second.first();
        }
    }

    return TpVariant();
}

void TpObject::installEventFilter(TpObject *filterObj)
{
    TpObjectData *objData = static_cast<TpObjectData *>(data_);
    if (!objData)
        return;

    objData->filterObject = filterObj;
}

void TpObject::uninstallEventFilter()
{
    TpObjectData *objData = static_cast<TpObjectData *>(data_);
    if (!objData)
        return;

    objData->filterObject = nullptr;
}

TpObject *TpObject::eventFilterObject()
{
    TpObjectData *objData = static_cast<TpObjectData *>(data_);
    if (!objData)
        return nullptr;

    return objData->filterObject;
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
    TpObjectData *objData = static_cast<TpObjectData *>(data_);
    if (!objData)
        return;

    disconnectAllSignal(objData);

    // 删除所有子节点
    for (const auto &child : objData->objectList)
    {
        child->uninstallEventFilter();
    }

    TpCoreApp::Inst()->sendDelete(this);
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
        set->top = nullptr;
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

    if (type == Tp::TP_FIXSCREEN_OBJECT || type == Tp::TP_MAIN_WINDOW_OBJECT /*|| type == Tp::TP_FLOAT_OBJECT*/)
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

                if (type == Tp::TP_FIXSCREEN_OBJECT || type == Tp::TP_MAIN_WINDOW_OBJECT /*|| type == Tp::TP_FLOAT_OBJECT*/)
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

void TpObject::initTpData()
{
    TpObjectData *objData = static_cast<TpObjectData *>(data_);
    objData->objectID = TpAutoObject::Inst()->selfCounterIncrease();
    objData->top = nullptr;
    objData->agent = nullptr;
}

void TpObject::addConnection(void *signal, std::function<void()> disconnector)
{
    TpObjectData *set = static_cast<TpObjectData *>(data_);

    std::lock_guard<std::mutex> lock(set->slotConnectMutex_);
    set->slotConnections_[signal].push_back(disconnector);
}