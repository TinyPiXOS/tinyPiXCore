#include "DesktopAppButton.h"
#include "TpPainter.h"
#include <TpEvent.h>

DesktopAppButton::DesktopAppButton(TpWidget *parent)
    : TpIconTopButton(parent)
{
    hollowWidget_ = new HollowWidget(this);
    hollowWidget_->setVisible(false);
}

DesktopAppButton::DesktopAppButton(const TpString &iconPath, const TpString &text, TpWidget *parent)
    : TpIconTopButton(iconPath, text, parent)
{
    hollowWidget_ = new HollowWidget(this);
    hollowWidget_->setVisible(false);
}

DesktopAppButton::~DesktopAppButton()
{
}

void DesktopAppButton::setRoundCorners(const uint32_t &round)
{
    TpIconTopButton::setRoundCorners(round);
    hollowWidget_->setRoundCorners(round);
}

void DesktopAppButton::setInstallProgress(const uint32_t &progress)
{
    hollowWidget_->setInstallProgress(progress);

    if (progress == 100)
        hollowWidget_->setVisible(false);
    else
        hollowWidget_->setVisible(true);
}

bool DesktopAppButton::onResizeEvent(TpResizeEvent *event)
{
    TpIconTopButton::onResizeEvent(event);

    hollowWidget_->setSize(width(), width());

    return true;
}

HollowWidget::HollowWidget(TpWidget *parent)
    : TpWidget(parent), installProgress_(100)
{
}

HollowWidget::~HollowWidget()
{
}

void HollowWidget::setInstallProgress(const uint32_t &progress)
{
    installProgress_ = progress;
}

bool HollowWidget::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    // 应用正在安装中；根据安装进度绘制遮罩层
    TpPainter *painter = event->painter();

    TpHollowMask hollowMaskData;
    TpHollowMask::PieHollow pieHollowData;
    pieHollowData.x = width() / 2.0;
    pieHollowData.y = height() / 2.0;
    pieHollowData.start = 0;
    pieHollowData.end = 360.0 * installProgress_ / 100;
    pieHollowData.radius = (width() / 2.0) - 3;
    hollowMaskData.addPieHollow(pieHollowData);

    painter->setPen(_RGBA(0, 0, 0, 125));
    painter->setBrush(TpBrush(_RGBA(0, 0, 0, 125)));

    painter->drawRect(0, 0, width(), width(), roundCorners(), hollowMaskData);

    return true;
}