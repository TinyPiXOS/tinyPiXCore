#include "TpObject.h"
#include "TpAutoObject.h"
#include "TpChildWidget.h"
#include "TpEvent.h"
#include "TpApp.h"
#include "TpCanvas.h"
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
#include "core/TpObjectFunction.hpp"

#include <cstring>
#include <mutex>
#include <iostream>

TpObject::TpObject(TpObject *parent)
{
	ItpObjectSet *set = new ItpObjectSet();

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

	set->alpha = 0xff;

	set->layout = nullptr;

	set->visible = false;

	memset(set->text, 0, OBJECT_MAX_TEXT_LENGTH);
	memset(&set->absoluteRect, 0, sizeof(ItpRect));
	memset(&set->logicalRect, 0, sizeof(ItpRect));
	memset(&set->tmp, 0, sizeof(ItpTempDef));

	this->objectSet = set;
}

TpObject::~TpObject()
{
	bool ret = TpApp::Inst()->isExistObject(this, true);

	if (!ret)
		return;

	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;

	if (!set)
		return;

	// {
	// 	std::lock_guard<std::mutex> lock(set->slotConnectMutex_);
	// 	// 对象销毁时断开所有关联连接
	// 	while (!set->slotConnections_.empty())
	// 	{
	// 		auto [signal, connId] = set->slotConnections_.back();
	// 		if (signal)
	// 		{
	// 			disconnect(signal)
	// 		}
	// 			signal->disconnect(connId);
	// 		connections_.pop_back();
	// 	}
	// }

	set->gMutex.lock();

	if (set->parent)
	{
		ItpObjectSet *parent_set = (ItpObjectSet *)set->parent->objectSets();
		delObject(parent_set, set->parent);
	}

	set->objectList.clear();
	set->gMutex.unlock();

	delete set;
}

void TpObject::setProperty(const TpString &_name, const TpVariant &_value)
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;

	set->objPropertyMap[_name] = _value;
}

TpVariant TpObject::property(const TpString &_name)
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;

	if (set->objPropertyMap.contains(_name))
		return set->objPropertyMap[_name];
	return TpVariant();
}

void TpObject::installEventFilter(TpObject *filterObj)
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	if (!set)
		return;

	set->filterObject = filterObj;
}

void TpObject::uninstallEventFilter()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	if (!set)
		return;

	set->filterObject = nullptr;
}

TpObject *TpObject::eventFilterObject()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
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
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;

	if (set)
	{
		broadObjectSetTop(this, set->top);
	}
}

TpObject *TpObject::find(int32_t id)
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
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
	TpApp::Inst()->sendDelete(this);
}

int32_t TpObject::objectID()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	int32_t id = TP_INVALIDATE_VALUE;

	if (set)
	{
		id = set->objectID;
	}

	return id;
}

TpList<TpObject *> &TpObject::objectList()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	TpList<TpObject *> *objectList = nullptr;

	if (set)
	{
		objectList = &set->objectList;
	}

	return (*objectList);
}

void TpObject::setParent(TpObject *parent)
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;

	if (!set)
		return;

	if (set->parent == parent)
		return;

	// 如果已有父指针， 移除以前的父指针
	if (set->parent)
	{
		ItpObjectSet *parentSet = (ItpObjectSet *)set->parent->objectSets();
		delObject(parentSet, this);
	}

	if (parent)
	{
		ItpObjectSet *parentSet = (ItpObjectSet *)parent->objectSets();
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
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	TpObject *parent = nullptr;

	if (set)
	{
		parent = set->parent;
	}

	return parent;
}

TpObject *TpObject::topObject()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	TpObject *top = nullptr, *pParent = nullptr;

	if (!set)
		return top;

	ItpObjectType type = objectType();

	if (type == TP_TOP_OBJECT || type == TP_FLOAT_OBJECT)
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
				ItpObjectType type = pParent->objectType();

				if (type == TP_TOP_OBJECT || type == TP_FLOAT_OBJECT)
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

IPitpObject *TpObject::objectSets()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	IPitpObject *sets = nullptr;

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
