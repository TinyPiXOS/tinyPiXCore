#include "TpPercentProgressBar.h"
#include "TpVector.h"
#include "TpFont.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include <cmath>

struct ItemData
{
    TpString name = "";
    double value = 0;
    int32_t color;
};

struct TpPercentProgressBarData
{
    double totalValue = 0;

    TpVector<ItemData> itemList;

    // 自带颜色模板容器
    TpVector<int32_t> colorTempList;
    uint32_t colorIndex = 0;

    TpFont *itemFont = new TpFont();

    ~TpPercentProgressBarData()
    {
        delete itemFont;
        itemFont = nullptr;
    }
};

TpPercentProgressBar::TpPercentProgressBar(TpWidget *parent)
    : TpWidget(parent)
{
    TpPercentProgressBarData *progressData = new TpPercentProgressBarData();

    progressData->colorTempList.emplace_back(_RGB(20, 71, 255));
    progressData->colorTempList.emplace_back(_RGB(255, 77, 79));
    progressData->colorTempList.emplace_back(_RGB(242, 190, 30));
    progressData->colorTempList.emplace_back(_RGB(190, 196, 202));
    progressData->colorTempList.emplace_back(_RGB(255, 112, 36));
    progressData->colorTempList.emplace_back(_RGB(0, 192, 144));

    tpShared<TpCssData> normalCss = currentStatusCss();

    progressData->itemFont->setFontSize(normalCss->fontSize());
    progressData->itemFont->setFontColor(normalCss->color());

    data_ = progressData;

    refreshBaseCss();

    // CSS中的圆角应用于进度条，而不应用于整个窗体
    // setRoundCorners(0);
}

TpPercentProgressBar::~TpPercentProgressBar()
{
    TpPercentProgressBarData *progressData = static_cast<TpPercentProgressBarData *>(data_);
    if (progressData)
    {
        delete progressData;
        progressData = nullptr;
    }
}

void TpPercentProgressBar::setTotalValue(const double &totalValue)
{
    TpPercentProgressBarData *progressData = static_cast<TpPercentProgressBarData *>(data_);
    progressData->totalValue = totalValue;

    update();
}

uint32_t TpPercentProgressBar::addItem(const TpString &name, const double &value)
{
    TpPercentProgressBarData *progressData = static_cast<TpPercentProgressBarData *>(data_);

    ItemData itemInfo;
    itemInfo.name = name;
    itemInfo.value = value;
    itemInfo.color = progressData->colorTempList.at(progressData->colorIndex);

    progressData->itemList.emplace_back(itemInfo);

    progressData->colorIndex++;
    if (progressData->colorIndex >= progressData->colorTempList.size())
        progressData->colorIndex = 0;

    update();

    return (progressData->itemList.size() - 1);
}

void TpPercentProgressBar::removeItem(const uint32_t &index)
{
    TpPercentProgressBarData *progressData = static_cast<TpPercentProgressBarData *>(data_);
    progressData->itemList.remove(index);
    update();
}

void TpPercentProgressBar::setItemText(const uint32_t &index, const TpString &text)
{
    TpPercentProgressBarData *progressData = static_cast<TpPercentProgressBarData *>(data_);

    if (index >= progressData->itemList.size())
        return;

    progressData->itemList[index].name = text;
    update();
}

void TpPercentProgressBar::setItemColor(const uint32_t &index, const int32_t &color)
{
    TpPercentProgressBarData *progressData = static_cast<TpPercentProgressBarData *>(data_);

    if (index >= progressData->itemList.size())
        return;

    progressData->itemList[index].color = color;
    update();
}

void TpPercentProgressBar::setItemValue(const uint32_t &index, const double &value)
{
    TpPercentProgressBarData *progressData = static_cast<TpPercentProgressBarData *>(data_);

    if (index >= progressData->itemList.size())
        return;

    progressData->itemList[index].value = value;
    update();
}

bool TpPercentProgressBar::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpPercentProgressBarData *progressData = static_cast<TpPercentProgressBarData *>(data_);
    if (progressData->totalValue == 0)
        return true;

    if (progressData->itemList.size() == 0)
        return true;

    TpPainter *paintCanvas = event->painter();
    tpShared<TpCssData> normalCss = currentStatusCss();

    // 绘制进度条底色
    uint32_t progressHeight = height() - progressData->itemFont->pixelHeight() - normalCss->gap();

    paintCanvas->setPen(normalCss->subColor());
    paintCanvas->setBrush(TpBrush(normalCss->subColor()));

    paintCanvas->drawRect(0, 0, width(), progressHeight, normalCss->roundCorners());

    // 进度条item颜色之间有间隔，绘制色块宽度要排除间隔
    uint32_t actualProgressWidth = width() - 2 * (progressData->itemList.size() - 1);

    // 依次绘制色块
    int32_t curItemX = 0;
    int32_t curItemContentX = 18;

    uint32_t itemContentY = progressHeight + normalCss->gap();

    uint32_t itemContentCirrleWidth = progressData->itemFont->pixelHeight() - 4;

    double curTotalValue = 0;
    for (int i = 0; i < progressData->itemList.size(); ++i)
    {
        ItemData itemInfo = progressData->itemList.at(i);
        curTotalValue += itemInfo.value;

        int32_t curItemWidth = (1.0 * itemInfo.value / progressData->totalValue) * actualProgressWidth;

        uint32_t endX = curItemX + curItemWidth;
        if (endX > width())
        {
            curItemWidth = curItemWidth - (endX - width());
            if (curItemWidth < 0)
                curItemWidth = 0;
        }

        paintCanvas->setPen(itemInfo.color);
        paintCanvas->setBrush(TpBrush(itemInfo.color));

        // 第一个要绘制一半圆角，一半方角
        if (i == 0)
        {
            paintCanvas->drawRect(curItemX, 0, curItemWidth, progressHeight, progressHeight / 2.0);

            // 绘制方角
            paintCanvas->drawRect(curItemX + curItemWidth - progressHeight / 2.0, 0, progressHeight, progressHeight);
        }
        else if (i == (progressData->itemList.size() - 1))
        {
            paintCanvas->setPen(itemInfo.color);
            paintCanvas->setBrush(TpBrush(itemInfo.color));

            if (std::fabs(curTotalValue - progressData->totalValue) < 1e-3)
            {
                // 最后一个如果刚好绘制到末尾，也要一半圆角，一半方角
                paintCanvas->drawRect(curItemX, 0, curItemWidth, progressHeight, progressHeight / 2.0);
            }
            else
            {
                paintCanvas->drawRect(curItemX, 0, curItemWidth, progressHeight);
            }
        }
        else
        {
            paintCanvas->drawRect(curItemX, 0, curItemWidth, progressHeight);
        }

        // 绘制item的提示文本
        progressData->itemFont->setText(itemInfo.name);
        paintCanvas->drawRect(curItemContentX, itemContentY + 2, itemContentCirrleWidth, height() - 4 - itemContentY, itemContentCirrleWidth / 2.0);

        curItemContentX += itemContentCirrleWidth + 5;

        paintCanvas->drawText(*progressData->itemFont, curItemContentX, itemContentY);

        curItemContentX += 18 + progressData->itemFont->pixelWidth();
        // if (curItemContentX > (width() - 18))
        // {
        //     curItemContentX = 18;
        //     itemContentY
        // }
        curItemX += curItemWidth + 2;
    }

    return true;
}

bool TpPercentProgressBar::onResizeEvent(TpResizeEvent *event)
{
    TpWidget::onResizeEvent(event);

    return true;
}
