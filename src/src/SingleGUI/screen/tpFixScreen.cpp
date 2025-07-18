/* liucy has been here，but nothing to see and nothing left ^_^!*/

/*
** Copyright (c) 2007-2021 By Alexander.King.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
#include "tpFixScreen.h"
#include "tpApp.h"
#include "tpDef.h"
#include "tpColors.h"
#include <tinyPiXWF.h>
#include <mutex>
#include "Utils/tpCssParser.h"
#include "SingleGUI/tpDefaultCss.h"
#include "tpString.h"
#include "tpVariant.h"
#include "tpDefaultCss.h"

struct tpFixScreenData
{
	uint8_t alpha;
	uint32_t color;
	int32_t attr;

	// std::shared_ptr<tpCssParser> tpCssParser_;

	tpFixScreenData()
		: alpha(0), color(0), attr(0)
	{
		// tpCssParser_ = std::make_shared<tpCssParser>(defaultCssStr());
	}
};

tpFixScreen::tpFixScreen(const char *type)
	: tpScreen(type)
{
	tpFixScreenData *screenData = new tpFixScreenData();
	data_ = screenData;

	// tpApp::Inst()->sendRegister(this);

	if (this->objectType() != TP_TOP_OBJECT)
	{
		tpApp::Inst()->sendDelete(this);
	}

	ItpObjectSet *set = (ItpObjectSet *)this->objectSets();
	if (set)
	{
		uint32_t rW = 0, rH = 0;
		tinyPiX_wf_get_rotate_metrics(set->agent, &rW, &rH);

		set->absoluteRect.x = 0;
		set->absoluteRect.y = 0;
		set->absoluteRect.w = rW;
		set->absoluteRect.h = rH;

		set->logicalRect.x = 0;
		set->logicalRect.y = 0;
		set->logicalRect.w = rW;
		set->logicalRect.h = rH;

		screenData->alpha = 0xff;
		screenData->color = tpColors::Black;
		screenData->attr = tpFixScreen::ITP_POP_STYLE;

		this->setVScreenAttribute(screenData->alpha, screenData->color, screenData->attr);
	}

	set->top = this->topObject();
}

tpFixScreen::~tpFixScreen()
{
	tpFixScreenData *screenData = static_cast<tpFixScreenData *>(data_);
	if (screenData)
	{
		delete screenData;
		screenData = nullptr;
		data_ = nullptr;
	}
}

ItpObjectType tpFixScreen::objectType()
{
	return TP_TOP_OBJECT;
}

int32_t tpFixScreen::setVScreenAttribute(uint8_t alpha, uint32_t color, int32_t screenAttr)
{
	tpFixScreenData *screenData = static_cast<tpFixScreenData *>(data_);
	if (!screenData)
		return false;

	switch (screenAttr)
	{
	case tpFixScreen::ITP_FULL_STYLE:
	case tpFixScreen::ITP_POP_STYLE:
	{
	}
	break;
	default:
		return false;
	}

	ItpObjectSet *set = (ItpObjectSet *)this->objectSets();

	if (set)
	{
		screenData->alpha = alpha;
		screenData->color = color;
		screenData->attr = screenAttr;

		return tinyPiX_wf_send_app_state(set->agent, TP_INVALIDATE_VALUE, this->visible(), this->objectActive(), color, alpha, screenAttr);
	}

	return false;
}

void tpFixScreen::setRect(tpRect &rect)
{
}

void tpFixScreen::setRect(tpRect *rect)
{
}

void tpFixScreen::setRect(ItpRect &rect)
{
}

void tpFixScreen::setRect(ItpRect *rect)
{
}

void tpFixScreen::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
}

void tpFixScreen::setPosition(int32_t x, int32_t y)
{
}

void tpFixScreen::setBeMoved(bool moved)
{
}

bool tpFixScreen::moved()
{
	return false;
}

void tpFixScreen::setAlpha(const uint8_t& alpha)
{
}

uint8_t tpFixScreen::alpha()
{
	return 0xff;
}

void tpFixScreen::setColorKey(bool enable, uint32_t colorKey)
{
}

bool tpFixScreen::colorKeyEnable()
{
	return false;
}

uint32_t tpFixScreen::colorKey()
{
	return 0;
}

void tpFixScreen::setVarShape(void *shape)
{
}

void *tpFixScreen::varShape()
{
	return nullptr;
}

bool tpFixScreen::onRotateEvent(tpObjectRotateEvent *event)
{
	tpFixScreenData *screenData = static_cast<tpFixScreenData *>(data_);
	if (!screenData)
		return false;

	return this->setVScreenAttribute(screenData->alpha, screenData->color, screenData->attr);
}

bool tpFixScreen::onActiveEvent(tpObjectActiveEvent *event)
{
	tpFixScreenData *screenData = static_cast<tpFixScreenData *>(data_);
	if (!screenData)
		return false;

	return this->setVScreenAttribute(screenData->alpha, screenData->color, screenData->attr);
}
