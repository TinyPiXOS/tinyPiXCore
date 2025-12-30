#include "TpToast.h"
#include "TpImage.h"
#include "TpFont.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpTimer.h"

struct TpToastData
{
    TpFont textFont;
    TpString text = "";
    TpImage icon;

    int32_t durationMs;

    TpWidget *toastWidget = nullptr;

    TpTimer *closeTimer = nullptr;
};

TpToast::TpToast() : TpDialog()
{
    TpToastData *toastData = new TpToastData();
    data_ = toastData;

    toastData->closeTimer = new TpTimer();
    connect(toastData->closeTimer, timeout, [this]()
            { close(); });

    setDuaration();

    refreshBaseCss();
}

TpToast::~TpToast()
{
    TpToastData *toastData = static_cast<TpToastData *>(data_);
    if (toastData)
    {
        delete toastData;
        toastData = nullptr;
        data_ = nullptr;
    }
}

void TpToast::exec()
{
    show();
}

void TpToast::setDuaration(int32_t timeMs)
{
    TpToastData *toastData = static_cast<TpToastData *>(data_);
    toastData->durationMs = timeMs;
}

void TpToast::setText(const TpString &text)
{
    TpToastData *toastData = static_cast<TpToastData *>(data_);
    toastData->text = text;
    toastData->textFont.setText(text);

    if (!toastData->toastWidget)
    {
        int32_t newWidth = 0;
        if (!toastData->icon.isNull())
        {
            newWidth += ((height() - toastData->icon.height()) / 2.0) * 2 + toastData->icon.width();
        }

        newWidth += toastData->textFont.pixelWidth() * 1.2;
        setWidth(newWidth);

        update();
    }
}

void TpToast::setIcon(IconType type)
{
    TpToastData *toastData = static_cast<TpToastData *>(data_);

    if (!toastData->toastWidget)
    {
        int32_t newWidth = 0;
        if (!toastData->icon.isNull())
        {
            newWidth += ((height() - toastData->icon.height()) / 2.0) * 2 + toastData->icon.width();
        }

        newWidth += toastData->textFont.pixelWidth() * 1.2;
        setWidth(newWidth);

        update();
    }
}

void TpToast::setIcon(const TpString &iconPath)
{
    TpToastData *toastData = static_cast<TpToastData *>(data_);
    toastData->icon = TpImage(iconPath);
    toastData->icon = toastData->icon.scaled(TpSize(height() * 0.6, height() * 0.6));

    if (!toastData->toastWidget)
    {
        int32_t newWidth = 0;
        if (!toastData->icon.isNull())
        {
            newWidth += ((height() - toastData->icon.height()) / 2.0) * 2 + toastData->icon.width();
        }

        newWidth += toastData->textFont.pixelWidth() * 1.2;
        setWidth(newWidth);

        update();
    }
}

void TpToast::setWidget(TpWidget *widget)
{
    TpToastData *toastData = static_cast<TpToastData *>(data_);
    if (toastData->toastWidget)
    {
        toastData->toastWidget->setParent(nullptr);
        toastData->toastWidget = nullptr;
    }

    if (widget)
    {
        widget->setParent(this);
        widget->setRoundCorners(roundCorners());
        widget->setSize(this->size());
        widget->move(0, 0);
    }

    toastData->toastWidget = widget;

    update();
}

TpWidget *TpToast::widget()
{
    TpToastData *toastData = static_cast<TpToastData *>(data_);
    return toastData->toastWidget;
}

void TpToast::setVisible(bool visible)
{
    TpSize screenSize = this->screenSize();

    move((screenSize.width() - width()) / 2.0, screenSize.height() * 0.06);

    TpDialog::setVisible(visible);

    TpToastData *toastData = static_cast<TpToastData *>(data_);
    if (visible)
    {
        toastData->closeTimer->setInterval(toastData->durationMs);
        toastData->closeTimer->start();
    }
    else
    {
        toastData->closeTimer->stop();
    }
}

bool TpToast::onPaintEvent(TpPaintEvent *event)
{
    TpDialog::onPaintEvent(event);

    TpToastData *toastData = static_cast<TpToastData *>(data_);

    TpPainter *painter = event->painter();

    int32_t textX = 0;

    if (!toastData->icon.isNull())
    {
        int32_t centerY = (height() - toastData->icon.height()) / 2.0;
        painter->drawImage(TpPoint(centerY, centerY), toastData->icon);

        textX += centerY * 2 + toastData->icon.width();

        toastData->textFont.setAlign(Tp::AlignLeft | Tp::AlignVCenter);
    }
    else
    {
        toastData->textFont.setAlign(Tp::AlignCenter);
    }

    if (!toastData->text.empty())
    {
        tpShared<TpCssData> currentCss = currentStatusCss();
        toastData->textFont.setText(toastData->text);
        toastData->textFont.setFontColor(currentCss->color());
        toastData->textFont.setFontSize(currentCss->fontSize());
        toastData->textFont.setLayout(width() - textX, height());
        toastData->textFont.setWrap(true);

        painter->drawText(toastData->textFont, textX, 0);
    }

    return true;
}
