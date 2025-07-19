#include "tpRadioButton.h"
#include "tpEvent.h"
#include "tpColors.h"
#include "tpCanvas.h"
#include "tpRect.h"
#include "tpFont.h"
#include "tpString.h"
#include <cstring>
#include <list>

struct tpRadioButtonData
{
	bool enableFit;
	bool mouseActive;

	int32_t space;
	tpFont *font;

	tpRadioButtonData()
	{
	}
};

tpRadioButton::tpRadioButton(tpChildWidget *parent) : tpChildWidget(parent)
{
	tpRadioButtonData *set = new tpRadioButtonData();

	if (!set)
		return;

	set->enableFit = false;
	set->mouseActive = false;
	set->space = 1;
	set->font = new tpFont();

	setEnableBackGroundImage(false);
	setEnableBackGroundColor(false);
	setEnabledBorderColor(false);

	data_ = set;

	setCheckable(true);

	refreshBaseCss();
}

tpRadioButton::tpRadioButton(const tpString &text, tpChildWidget *parent)
	: tpChildWidget(parent)
{
	tpRadioButtonData *set = new tpRadioButtonData();

	if (!set)
		return;

	set->enableFit = false;
	set->mouseActive = false;
	set->space = 1;
	set->font = new tpFont();

	setEnableBackGroundImage(false);
	setEnableBackGroundColor(false);
	setEnabledBorderColor(false);

	data_ = set;

	setText(text);
	setCheckable(true);

	refreshBaseCss();
}

tpRadioButton::~tpRadioButton()
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->data_;

	if (set)
	{
		if (set->font)
		{
			delete set->font;
		}

		delete set;
		set = nullptr;
	}
}

void tpRadioButton::setAutoFit(bool enable)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->data_;

	if (!set)
		return;

	set->enableFit = enable;
	if (enable)
	{
		ItpSize size = set->font->pixelSize();
		this->setRect(this->rect().x, this->rect().y, size.w + size.h / 2 + set->space, size.h);
	}
}

void tpRadioButton::setSpacing(uint32_t space)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->data_;

	if (set)
	{
		set->space = space;
	}
}

void tpRadioButton::setRect(const int32_t &x, const int32_t &y, const uint32_t &w, const uint32_t &h)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->data_;

	if (!set)
		return;

	if (set->enableFit)
	{
		ItpSize size = set->font->pixelSize();
		tpChildWidget::setRect(x, y, size.w + size.h / 2 + set->space, size.h);
		return;
	}

	tpChildWidget::setRect(x, y, w, h);
}

void tpRadioButton::setText(const tpString &text)
{
	if (text.empty())
		return;

	tpChildWidget::setText(text);
	tpRadioButtonData *set = (tpRadioButtonData *)this->data_;

	if (!set)
		return;

	set->font->setText(text);
	if (set->enableFit)
	{
		ItpSize size = set->font->pixelSize();
		this->setRect(this->rect().x, this->rect().y, size.w + size.h / 4 + set->space, size.h);
	}
}

tpFont *tpRadioButton::font()
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->data_;
	tpFont *font = nullptr;

	if (set)
	{
		font = set->font;
	}

	return font;
}

struct drawArgs
{
	bool checked;

	int32_t bcolor;
	int32_t ccolor;

	double rad;

	tpObjectPaintEvent *event;
	tpRadioButtonData *set;
};

static inline void draw(tpCanvas *canvas, cairo_t *cr, cairo_surface_t *cairo_surface, int32_t offsetX, int32_t offsetY, void *args)
{
	drawArgs *arg = (drawArgs *)args;
	ItpSize size = arg->set->font->pixelSize();
	int32_t lineWidth = TP_MAX(1, arg->rad / 8);
	double cx = (arg->event->rect().w - size.w) / 2.0, cy = (arg->event->rect().h - size.h) / 2.0;
	cx = TP_MAX(cx, arg->rad);

	cairo_set_line_width(cr, lineWidth);
	double r = _R(arg->bcolor) / 255.0, g = _G(arg->bcolor) / 255.0, b = _B(arg->bcolor) / 255.0, a = _A(arg->bcolor) / 255.0;
	cairo_set_source_rgba(cr, r, g, b, a);

	cairo_arc(cr, cx + offsetX, cy + offsetY + 9 * size.h / 16.0, arg->rad, 0, 2 * M_PI);
	cairo_stroke(cr);

	if (arg->checked)
	{
		r = _R(arg->ccolor) / 255.0;
		g = _G(arg->ccolor) / 255.0;
		b = _B(arg->ccolor) / 255.0;
		a = _A(arg->ccolor) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_arc(cr, cx + offsetX, cy + offsetY + 9 * size.h / 16.0, arg->rad / 2.0, 0, 2 * M_PI);
		cairo_stroke_preserve(cr);
		cairo_fill(cr);
	}

	canvas->renderText(*arg->set->font, cx + arg->rad + arg->set->space, cy);
}

bool tpRadioButton::onMousePressEvent(tpMouseEvent *event)
{
	tpChildWidget::onMousePressEvent(event);

	tpRadioButtonData *set = (tpRadioButtonData *)this->data_;
	if (!set)
		return true;

	set->mouseActive = true;

	update();

	return true;
}

bool tpRadioButton::onMouseRleaseEvent(tpMouseEvent *event)
{
	tpChildWidget::onMouseRleaseEvent(event);

	tpRadioButtonData *set = (tpRadioButtonData *)this->data_;
	if (!set)
		return true;

	set->mouseActive = false;

	onClicked.emit(checked());

	return true;
}

bool tpRadioButton::onPaintEvent(tpObjectPaintEvent *event)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->data_;
	if (!set)
		return true;

	tpChildWidget::onPaintEvent(event);

	tpShared<tpCssData> curCssData = currentStatusCss();

	tpCanvas *canvas = event->canvas();
	ItpSize size = set->font->pixelSize();
	// ItpRect eventRect = event->rect();
	// double cx = (event->rect().w - size.w) / 2.0, cy = (event->rect().h - size.h) / 2.0;

	drawArgs args;

	args.checked = checked();

	args.bcolor = curCssData->borderColor();
	args.ccolor = curCssData->backgroundColor();

	set->font->setFontColor(curCssData->color(), curCssData->color());

	args.event = event;
	args.set = set;

	args.rad = size.h / 4.0;

	event->canvas()->customizedCarioMethod(draw, &args);

	return true;
}
