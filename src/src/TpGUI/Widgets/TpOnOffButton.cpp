
#include "TpOnOffButton.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpFont.h"
#include "TpLabel.h"
#include "TpAnimation.h"

// 滑块距离边界距离
const uint32_t RectMargin = 3;

struct TpOnOffButtonData
{
    // TpFont *font = nullptr;
    TpOnOffButton::Direction direction = TpOnOffButton::TP_HORIZONTAL;
    bool onOff = false;

    int32_t onColor = _RGB(255, 255, 255);
    int32_t offColor = _RGB(255, 255, 255);

    int32_t onBackColor = _RGB(140, 29, 235);
    int32_t offBackColor = _RGB(217, 217, 217);

    // 滑块Label；使用label来实现动画效果
    TpLabel *onOffRectLabel = nullptr;
    // TpString onText = "";
    // TpString offText = "";

    ~TpOnOffButtonData()
    {
        onOffRectLabel->deleteLater();
        // delete font;
        // font = nullptr;
    }
};

TpOnOffButton::TpOnOffButton(TpWidget *parent, const Direction &horizontal)
    : TpWidget(parent)
{
    TpOnOffButtonData *btnData = new TpOnOffButtonData();
    data_ = btnData;

    btnData->onOffRectLabel = new TpLabel(this);
    btnData->onOffRectLabel->setBackGroundColor(btnData->offColor);
    btnData->onOffRectLabel->installEventFilter(this);
    btnData->onOffRectLabel->move(RectMargin, RectMargin);

    btnData->direction = horizontal;

    // btnData->font = new TpFont();
    // btnData->font->setFontColor(_RGB(0, 0, 0),_RGB(0, 0, 0));

    setEnableBackGroundColor(false);
    setEnabledBorderColor(false);
    setOnOff(false);
}

TpOnOffButton::~TpOnOffButton()
{
    TpOnOffButtonData *btnData = static_cast<TpOnOffButtonData *>(data_);
    if (btnData)
    {
        delete btnData;
        btnData = nullptr;
        data_ = nullptr;
    }
}

void TpOnOffButton::setOnOff(bool onOff)
{
    TpOnOffButtonData *btnData = static_cast<TpOnOffButtonData *>(data_);

    if (!btnData)
        return;

    if (btnData->onOff != onOff)
    {
        btnData->onOff = onOff;

        TpAnimation *swtichAnimation = new TpAnimation(btnData->onOffRectLabel, TpAnimation::Pos);
        swtichAnimation->setStartValue(btnData->onOffRectLabel->pos());
        swtichAnimation->setDuration(150);

        // 更新滑块位置
        if (btnData->direction == TpOnOffButton::TP_HORIZONTAL)
        {
            if (btnData->onOff)
            {
                swtichAnimation->setEndValue(TpPoint(width() - btnData->onOffRectLabel->width() - RectMargin, RectMargin));
            }
            else
            {
                swtichAnimation->setEndValue(TpPoint(RectMargin, RectMargin));
            }
        }
        else
        {
            if (btnData->onOff)
            {
                swtichAnimation->setEndValue(TpPoint(RectMargin, height() - btnData->onOffRectLabel->height() - RectMargin));
            }
            else
            {
                swtichAnimation->setEndValue(TpPoint(RectMargin, RectMargin));
            }
        }

        swtichAnimation->start();

        update();
    }
}

bool TpOnOffButton::onOff()
{
    TpOnOffButtonData *btnData = static_cast<TpOnOffButtonData *>(data_);
    return btnData->onOff;
}

void TpOnOffButton::setOnColor(const uint32_t &color)
{
    TpOnOffButtonData *btnData = static_cast<TpOnOffButtonData *>(data_);
    btnData->onColor = color;
}

void TpOnOffButton::setOffColor(const uint32_t &color)
{
    TpOnOffButtonData *btnData = static_cast<TpOnOffButtonData *>(data_);
    btnData->offColor = color;
}

void TpOnOffButton::setOnBackColor(const uint32_t &color)
{
    TpOnOffButtonData *btnData = static_cast<TpOnOffButtonData *>(data_);
    btnData->onBackColor = color;
}

void TpOnOffButton::setOffBackColor(const uint32_t &color)
{
    TpOnOffButtonData *btnData = static_cast<TpOnOffButtonData *>(data_);
    btnData->offBackColor = color;
}

bool TpOnOffButton::onMousePressEvent(TpMouseEvent *event)
{
    TpWidget::onMousePressEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    TpOnOffButtonData *btnData = static_cast<TpOnOffButtonData *>(data_);
    if (!btnData)
        return true;

    setOnOff(!onOff());
    onClicked.emit(onOff());

    return true;
}

bool TpOnOffButton::onPaintEvent(TpPaintEvent *event)
{
    TpOnOffButtonData *btnData = static_cast<TpOnOffButtonData *>(data_);
    if (!btnData)
        return true;

    TpWidget::onPaintEvent(event);

    TpPainter *canvas = event->painter();

    // 绘制背景色
    int32_t backColor = btnData->onOff ? btnData->onBackColor : btnData->offBackColor;
    canvas->setBrush(TpBrush(backColor));
    canvas->drawRect(0, 0, width(), height(), roundCorners());

    return true;
}

bool TpOnOffButton::onResizeEvent(TpResizeEvent *event)
{
    TpOnOffButtonData *btnData = static_cast<TpOnOffButtonData *>(data_);
    if (!btnData)
        return true;

    TpWidget::onResizeEvent(event);

    // 尺寸变化同步更改圆角值
    if (btnData->direction == TpOnOffButton::TP_HORIZONTAL)
    {
        setRoundCorners(height() * 0.5);

        int32_t rectWidthHeight = height() - RectMargin * 2;
        if (rectWidthHeight < 0)
            rectWidthHeight = height();
        btnData->onOffRectLabel->setSize(rectWidthHeight, rectWidthHeight);
        btnData->onOffRectLabel->setRoundCorners(rectWidthHeight / 2.0);
    }
    else
    {
        setRoundCorners(width() * 0.5);

        int32_t rectWidthHeight = width() - RectMargin * 2;
        if (rectWidthHeight < 0)
            rectWidthHeight = width();
        btnData->onOffRectLabel->setSize(rectWidthHeight, rectWidthHeight);
        btnData->onOffRectLabel->setRoundCorners(rectWidthHeight / 2.0);
    }

    return true;
}

bool TpOnOffButton::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }

    return false;
}
