#include "tpObject.h"
#include "tpAutoObject.h"
#include "tpChildWidget.h"
#include "tpEvent.h"
#include "tpApp.h"
#include "tpCanvas.h"
#include "tpLayout.h"
#include "tpDef.h"
#include "tpObjectStack.h"
#include <tpSurface.h>
#include <tpColors.h>
#include <tpRect.h>
#include <tpPoint.h>
#include <tinyPiXUtils.h>
#include <tpHash.h>
#include <tpString.h>
#include <tpVariant.h>
#include "core/tpObjectFunction.hpp"

#include <cstring>
#include <mutex>
#include <iostream>

tpObject::tpObject(tpObject *parent)
{
	ItpObjectSet *set = new ItpObjectSet();

	if (!set)
		return;

	set->offsetX = 0;
	set->offsetY = 0;

	set->backColor = _RGB(248, 248, 248);
	// set->backColor = tpApp::Inst()->appConfigSet()->bkcolor;
	set->objectID = tpAutoObject::Inst()->selfCounterIncrease();

	set->reserveImage = nullptr;
	set->cacheImage = nullptr;

	set->top = nullptr;

	set->varSize = 0;
	set->varShape = nullptr;
	set->agent = nullptr;

	set->enableColor = true;
	// set->enableColor = tpApp::Inst()->appConfigSet()->enable;
	set->enableImage = true;

	set->alpha = 0xff;
	set->colorKey = 0x000000000;
	set->enableColorKey = false;

	set->layout = nullptr;

	set->visible = false;
	set->enableRotate = false;

	memset(set->text, 0, OBJECT_MAX_TEXT_LENGTH);
	memset(&set->absoluteRect, 0, sizeof(ItpRect));
	memset(&set->logicalRect, 0, sizeof(ItpRect));
	memset(&set->tmp, 0, sizeof(ItpTempDef));

	this->objectSet = set;
}

tpObject::~tpObject()
{
	bool ret = tpApp::Inst()->isExistObject(this, true);

	if (ret)
	{
		ItpObjectSet *set = (ItpObjectSet *)this->objectSet;

		if (set)
		{
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
	}
}

void tpObject::setProperty(const tpString &_name, const tpVariant &_value)
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;

	set->objPropertyMap[_name] = _value;
}

tpVariant tpObject::property(const tpString &_name)
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;

	if (set->objPropertyMap.contains(_name))
		return set->objPropertyMap[_name];
	return tpVariant();
}

void tpObject::installEventFilter(tpObject *filterObj)
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	if (!set)
		return;

	set->filterObject = filterObj;
}

void tpObject::uninstallEventFilter()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	if (!set)
		return;

	set->filterObject = nullptr;
}

tpObject *tpObject::eventFilterObject()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	if (!set)
		return nullptr;

	return set->filterObject;
}

bool tpObject::eventFilter(tpObject *watched, tpEvent *event)
{
	return false;
}

void tpObject::broadSetTop()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;

	if (set)
	{
		broadObjectSetTop(this, set->top);
	}
}

tpObject *tpObject::find(int32_t id)
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	tpObject *object = this;

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

void tpObject::deleteLater()
{
	tpApp::Inst()->sendDelete(this);
}

int32_t tpObject::objectID()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	int32_t id = TP_INVALIDATE_VALUE;

	if (set)
	{
		id = set->objectID;
	}

	return id;
}

tpList<tpObject *> &tpObject::objectList()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	tpList<tpObject *> *objectList = nullptr;

	if (set)
	{
		objectList = &set->objectList;
	}

	return (*objectList);
}

void tpObject::setParent(tpObject *parent)
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

tpObject *tpObject::parent()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	tpObject *parent = nullptr;

	if (set)
	{
		parent = set->parent;
	}

	return parent;
}

tpObject *tpObject::topObject()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	tpObject *top = nullptr, *pParent = nullptr;

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

IPitpObject *tpObject::objectSets()
{
	ItpObjectSet *set = (ItpObjectSet *)this->objectSet;
	IPitpObject *sets = nullptr;

	if (set)
	{
		sets = set;
	}

	return sets;
}

void *tpObject::operator new(size_t size)
{
	void *object = (void *)malloc(size);

	if (object == nullptr)
	{
		return NULL;
	}

	tpAutoObject *helper = tpAutoObject::Inst();

	if (helper)
	{
		helper->addObjectLife(object);
	}

	return object;
}

void *tpObject::operator new[](size_t size)
{
	void *object = (void *)malloc(size);

	if (object == nullptr)
	{
		return NULL;
	}

	tpAutoObject *helper = tpAutoObject::Inst();

	if (helper)
	{
		helper->addObjectLife(object);
	}

	return object;
}

void tpObject::operator delete(void *ptr)
{
	if (!ptr)
		return;

	tpAutoObject *helper = tpAutoObject::Inst();

	if (!helper)
		return;

	if (helper->isExist(ptr))
	{
		helper->removeObjectLife(ptr);
		free(ptr);
	}
}

void tpObject::operator delete[](void *ptr)
{
	if (!ptr)
		return;

	tpAutoObject *helper = tpAutoObject::Inst();

	if (!helper)
		return;

	if (helper->isExist(ptr))
	{
		helper->removeObjectLife(ptr);
		free(ptr);
	}
}
