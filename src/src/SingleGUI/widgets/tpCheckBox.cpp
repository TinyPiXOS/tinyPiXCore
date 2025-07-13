#include "tpCheckBox.h"
#include "tpCanvas.h"
#include "tpEvent.h"
#include "tpSurface.h"

struct tpCheckBoxData
{
	tpString text = "";
	tpFont *textFont = new tpFont();

	// 勾选时的对钩图片
	tpShared<tpSurface> checkIcon = tpMakeShared<tpSurface>();
	tpShared<tpSurface> originIcon = tpMakeShared<tpSurface>();

	~tpCheckBoxData()
	{
		delete textFont;
		textFont = nullptr;
	}
};

tpCheckBox::tpCheckBox(tpChildWidget *parent)
	: tpChildWidget(parent)
{
	tpShared<tpCssData> normalCss = currentStatusCss();

	tpCheckBoxData *cbxData = new tpCheckBoxData();

	cbxData->textFont->setFontColor(normalCss->color(), normalCss->color());
	cbxData->textFont->setFontSize(normalCss->fontSize());

	cbxData->originIcon->fromFile("/usr/res/tinyPiX/勾选-白.png");
	cbxData->checkIcon = cbxData->originIcon;

	data_ = cbxData;

	// refreshBaseCss();
	setMinimumSize(normalCss->minimumWidth(), normalCss->minimumHeight());
	setMaximumSize(normalCss->maximumWidth(), normalCss->maximumHeight());
	setSize(normalCss->width(), normalCss->height());

	setCheckable(true);
}

tpCheckBox::~tpCheckBox()
{
	tpCheckBoxData *cbxData = static_cast<tpCheckBoxData *>(data_);
	if (cbxData)
	{
		delete cbxData;
		cbxData = nullptr;
	}
}

void tpCheckBox::setText(const tpString &text)
{
	tpCheckBoxData *cbxData = static_cast<tpCheckBoxData *>(data_);

	cbxData->text = text;
	cbxData->textFont->setText(text);

	if (text.empty())
	{
		setMinumumWidth(height());
	}
	else
	{
		tpShared<tpCssData> normalCss = currentStatusCss();
		setMinumumWidth(height() + normalCss->gap() + cbxData->textFont->pixelWidth());
	}

	update();
}

tpFont *tpCheckBox::font()
{
	tpCheckBoxData *cbxData = static_cast<tpCheckBoxData *>(data_);

	return cbxData->textFont;
}

bool tpCheckBox::onMousePressEvent(tpMouseEvent *event)
{
	tpChildWidget::onMousePressEvent(event);

	if (event->button() != BUTTON_LEFT)
		return true;

	return true;
}

bool tpCheckBox::onMouseRleaseEvent(tpMouseEvent *event)
{
	tpChildWidget::onMousePressEvent(event);

	if (event->button() != BUTTON_LEFT)
		return true;

	onClicked.emit(checked());

	return true;
}

bool tpCheckBox::onResizeEvent(tpObjectResizeEvent *event)
{
	tpChildWidget::onResizeEvent(event);

	tpCheckBoxData *cbxData = static_cast<tpCheckBoxData *>(data_);
	cbxData->checkIcon = cbxData->originIcon->scaled(height() * 0.7, height() * 0.47);

	// std::cout << "tpCheckBox::onResizeEvent " << width() << "  " << height() << std::endl;

	if (cbxData->text.empty())
	{
		setMinumumWidth(height());
	}
	// else
	// {
	// 	tpShared<tpCssData> normalCss = currentStatusCss();
	// 	setMinumumWidth(height() + normalCss->gap() + cbxData->textFont->pixelWidth());
	// }

	return true;
}

bool tpCheckBox::onPaintEvent(tpObjectPaintEvent *event)
{
	// return true;
	// tpChildWidget::onPaintEvent(event);

	tpCanvas *paintCanvas = event->canvas();

	tpCheckBoxData *cbxData = static_cast<tpCheckBoxData *>(data_);

	// // 绘制圆角框
	tpShared<tpCssData> normalCss = currentStatusCss();

	if (!cbxData->text.empty())
	{
		tpShared<tpCssData> normalCss = currentStatusCss();
		setMinumumWidth(height() + normalCss->gap() + cbxData->textFont->pixelWidth());
	}

	uint32_t minRad = normalCss->roundCorners();

	uint32_t iconX = (height() - cbxData->checkIcon->width()) / 2.0;
	uint32_t iconY = (height() - cbxData->checkIcon->height()) / 2.0;

	if (minRad == 0)
	{
		// 选中态只绘制背景，未选中只绘制边框
		if (checked())
		{
			paintCanvas->box(0, 0, height(), height(), normalCss->backgroundColor());
			paintCanvas->paintSurface(iconX, iconY, cbxData->checkIcon);
		}
		else
		{
			paintCanvas->rectangle(0, 0, height(), height(), normalCss->borderColor(), normalCss->borderWidth());
		}
	}
	else
	{
		if (checked())
		{
			paintCanvas->roundedBox(0, 0, height(), height(), minRad, normalCss->backgroundColor());
			paintCanvas->paintSurface(iconX, iconY, cbxData->checkIcon);
		}
		else
		{
			paintCanvas->roundedRectangle(0, 0, height(), height(), minRad, normalCss->borderColor(), normalCss->borderWidth());
		}
	}

	if (!cbxData->text.empty())
	{
		cbxData->textFont->setText(cbxData->text);
		uint32_t fontY = (height() - cbxData->textFont->pixelHeight()) / 2.0;
		paintCanvas->renderText(*cbxData->textFont, height() + normalCss->gap(), fontY);
	}

	return true;
}

bool tpCheckBox::eventFilter(tpObject *watched, tpEvent *event)
{
	return false;
}
