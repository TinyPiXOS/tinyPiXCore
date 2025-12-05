
#include "TpSwitchButton.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpFont.h"
#include "TpLabel.h"
#include "TpAnimation.h"

// 滑块距离边界距离
const uint32_t RectMargin = 3;

struct TpSwitchButtonData
{
    // TpFont *font = nullptr;
    TpSwitchButton::Direction direction = TpSwitchButton::TP_HORIZONTAL;
    bool onOff = false;

    int32_t onColor = _RGB(255, 255, 255);
    int32_t offColor = _RGB(255, 255, 255);

    int32_t onBackColor = _RGB(140, 29, 235);
    int32_t offBackColor = _RGB(217, 217, 217);

    // 滑块Label；使用label来实现动画效果
    TpLabel *onOffRectLabel = nullptr;

    TpAnimation *switchAnimation = nullptr;

    ~TpSwitchButtonData()
    {
    }
};

TpSwitchButton::TpSwitchButton(TpWidget *parent, const Direction &horizontal)
    : TpWidget(parent)
{
    TpSwitchButtonData *btnData = new TpSwitchButtonData();
    data_ = btnData;

    btnData->onOffRectLabel = new TpLabel(this);
    btnData->onOffRectLabel->setBackGroundColor(btnData->offColor);
    btnData->onOffRectLabel->installEventFilter(this);
    btnData->onOffRectLabel->move(RectMargin, RectMargin);

    btnData->direction = horizontal;

    btnData->switchAnimation = new TpAnimation(btnData->onOffRectLabel, TpAnimation::Pos);
    btnData->switchAnimation->setDuration(150);

    setEnableBackGroundColor(false);
    setEnabledBorderColor(false);
    setOnOff(false);
}

TpSwitchButton::~TpSwitchButton()
{
    TpSwitchButtonData *btnData = static_cast<TpSwitchButtonData *>(data_);
    if (btnData)
    {
        delete btnData;
        btnData = nullptr;
        data_ = nullptr;
    }
}

void TpSwitchButton::setOnOff(bool onOff)
{
    TpSwitchButtonData *btnData = static_cast<TpSwitchButtonData *>(data_);

    if (!btnData)
        return;

    if (btnData->onOff != onOff)
    {
        btnData->onOff = onOff;

        btnData->switchAnimation->stop();
        btnData->switchAnimation->setStartValue(btnData->onOffRectLabel->pos());

        // 更新滑块位置
        if (btnData->direction == TpSwitchButton::TP_HORIZONTAL)
        {
            if (btnData->onOff)
            {
                btnData->switchAnimation->setEndValue(TpPoint(width() - btnData->onOffRectLabel->width() - RectMargin, RectMargin));
            }
            else
            {
                btnData->switchAnimation->setEndValue(TpPoint(RectMargin, RectMargin));
            }
        }
        else
        {
            if (btnData->onOff)
            {
                btnData->switchAnimation->setEndValue(TpPoint(RectMargin, height() - btnData->onOffRectLabel->height() - RectMargin));
            }
            else
            {
                btnData->switchAnimation->setEndValue(TpPoint(RectMargin, RectMargin));
            }
        }

        btnData->switchAnimation->start(TpAnimation::KeepWhenStopped);
    }
}

bool TpSwitchButton::onOff()
{
    TpSwitchButtonData *btnData = static_cast<TpSwitchButtonData *>(data_);
    return btnData->onOff;
}

void TpSwitchButton::setOnColor(const uint32_t &color)
{
    TpSwitchButtonData *btnData = static_cast<TpSwitchButtonData *>(data_);
    btnData->onColor = color;
}

void TpSwitchButton::setOffColor(const uint32_t &color)
{
    TpSwitchButtonData *btnData = static_cast<TpSwitchButtonData *>(data_);
    btnData->offColor = color;
}

void TpSwitchButton::setOnBackColor(const uint32_t &color)
{
    TpSwitchButtonData *btnData = static_cast<TpSwitchButtonData *>(data_);
    btnData->onBackColor = color;
}

void TpSwitchButton::setOffBackColor(const uint32_t &color)
{
    TpSwitchButtonData *btnData = static_cast<TpSwitchButtonData *>(data_);
    btnData->offBackColor = color;
}

bool TpSwitchButton::onMousePressEvent(TpMouseEvent *event)
{
    TpWidget::onMousePressEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    TpSwitchButtonData *btnData = static_cast<TpSwitchButtonData *>(data_);
    if (!btnData)
        return true;

    setOnOff(!onOff());
    onClicked.emit(onOff());

    return true;
}

bool TpSwitchButton::onPaintEvent(TpPaintEvent *event)
{
    TpSwitchButtonData *btnData = static_cast<TpSwitchButtonData *>(data_);
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

bool TpSwitchButton::onResizeEvent(TpResizeEvent *event)
{
    TpSwitchButtonData *btnData = static_cast<TpSwitchButtonData *>(data_);
    if (!btnData)
        return true;

    TpWidget::onResizeEvent(event);

    // 尺寸变化同步更改圆角值
    if (btnData->direction == TpSwitchButton::TP_HORIZONTAL)
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

bool TpSwitchButton::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }

    return false;
}
