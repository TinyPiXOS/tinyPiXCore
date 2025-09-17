#include "TpCheckBox.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpImage.h"

struct TpCheckBoxData
{
    TpString text = "";
    TpFont *textFont = new TpFont();

    // 勾选时的对钩图片
    TpImage checkIcon;
    TpImage originIcon;

    ~TpCheckBoxData()
    {
        delete textFont;
        textFont = nullptr;
    }
};

TpCheckBox::TpCheckBox(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    tpShared<TpCssData> normalCss = currentStatusCss();

    TpCheckBoxData *cbxData = new TpCheckBoxData();

    cbxData->textFont->setFontColor(normalCss->color(), normalCss->color());
    cbxData->textFont->setFontSize(normalCss->fontSize());

    cbxData->originIcon.load("/usr/res/tinyPiX/勾选-白.png");
    cbxData->checkIcon = cbxData->originIcon;

    data_ = cbxData;

    // refreshBaseCss();
    setMinimumSize(normalCss->minimumWidth(), normalCss->minimumHeight());
    setMaximumSize(normalCss->maximumWidth(), normalCss->maximumHeight());
    setSize(normalCss->width(), normalCss->height());

    setEnabledBorderColor(false);
    setCheckable(true);
}

TpCheckBox::~TpCheckBox()
{
    TpCheckBoxData *cbxData = static_cast<TpCheckBoxData *>(data_);
    if (cbxData)
    {
        delete cbxData;
        cbxData = nullptr;
    }
}

void TpCheckBox::setText(const TpString &text)
{
    TpCheckBoxData *cbxData = static_cast<TpCheckBoxData *>(data_);

    cbxData->text = text;
    cbxData->textFont->setText(text);

    if (text.empty())
    {
        setMinumumWidth(height());
    }
    else
    {
        tpShared<TpCssData> normalCss = currentStatusCss();
        setMinumumWidth(height() + normalCss->gap() + cbxData->textFont->pixelWidth());
    }

    update();
}

TpFont *TpCheckBox::font()
{
    TpCheckBoxData *cbxData = static_cast<TpCheckBoxData *>(data_);

    return cbxData->textFont;
}

bool TpCheckBox::onMousePressEvent(TpMouseEvent *event)
{
    TpChildWidget::onMousePressEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    return true;
}

bool TpCheckBox::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpChildWidget::onMouseRleaseEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    onClicked.emit(checked());

    return true;
}

bool TpCheckBox::onResizeEvent(TpResizeEvent *event)
{
    TpChildWidget::onResizeEvent(event);

    TpCheckBoxData *cbxData = static_cast<TpCheckBoxData *>(data_);
    cbxData->checkIcon = cbxData->originIcon.scaled(height() * 0.7, height() * 0.47);

    // std::cout << "TpCheckBox::onResizeEvent " << width() << "  " << height() << std::endl;

    if (cbxData->text.empty())
    {
        setMinumumWidth(height());
    }
    // else
    // {
    // 	tpShared<TpCssData> normalCss = currentStatusCss();
    // 	setMinumumWidth(height() + normalCss->gap() + cbxData->textFont->pixelWidth());
    // }

    return true;
}

bool TpCheckBox::onPaintEvent(TpPaintEvent *event)
{
    // return true;
    // TpChildWidget::onPaintEvent(event);

    TpPainter *paintCanvas = event->painter();

    TpCheckBoxData *cbxData = static_cast<TpCheckBoxData *>(data_);

    // // 绘制圆角框
    tpShared<TpCssData> normalCss = currentStatusCss();

    if (!cbxData->text.empty())
    {
        tpShared<TpCssData> normalCss = currentStatusCss();
        setMinumumWidth(height() + normalCss->gap() + cbxData->textFont->pixelWidth());
    }

    uint32_t minRad = normalCss->roundCorners();

    uint32_t iconX = (height() - cbxData->checkIcon.width()) / 2.0;
    uint32_t iconY = (height() - cbxData->checkIcon.height()) / 2.0;

    // 选中态只绘制背景，未选中只绘制边框
    if (checked())
    {
        paintCanvas->setBrush(TpBrush(normalCss->backgroundColor()));
        paintCanvas->drawRect(0, 0, height(), height(), minRad);
        paintCanvas->drawImage(iconX, iconY, cbxData->checkIcon);
    }
    else
    {
        paintCanvas->setBrush(TpBrush(tinyPiX::NoBrush));
        paintCanvas->setPen(normalCss->borderColor());
        paintCanvas->pen().setWidth(normalCss->borderWidth());
        paintCanvas->drawRect(0, 0, height(), height(), minRad);
    }

    if (!cbxData->text.empty())
    {
        cbxData->textFont->setText(cbxData->text);
        uint32_t fontY = (height() - cbxData->textFont->pixelHeight()) / 2.0;
        paintCanvas->drawText(*cbxData->textFont, height() + normalCss->gap(), fontY);
    }

    return true;
}

bool TpCheckBox::eventFilter(TpObject *watched, TpEvent *event)
{
    return false;
}
