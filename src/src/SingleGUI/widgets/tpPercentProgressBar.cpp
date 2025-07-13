#include "tpPercentProgressBar.h"
#include "tpVector.h"
#include "tpFont.h"
#include "tpEvent.h"
#include "tpCanvas.h"
#include <cmath>

struct ItemData
{
    tpString name = "";
    double value = 0;
    int32_t color;
};

struct tpPercentProgressBarData
{
    double totalValue = 0;

    tpVector<ItemData> itemList;

    // 自带颜色模板容器
    tpVector<int32_t> colorTempList;
    uint32_t colorIndex = 0;

    tpFont *itemFont = new tpFont();

    ~tpPercentProgressBarData()
    {
        delete itemFont;
        itemFont = nullptr;
    }
};

tpPercentProgressBar::tpPercentProgressBar(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpPercentProgressBarData *progressData = new tpPercentProgressBarData();

    progressData->colorTempList.emplace_back(_RGB(20, 71, 255));
    progressData->colorTempList.emplace_back(_RGB(255, 77, 79));
    progressData->colorTempList.emplace_back(_RGB(242, 190, 30));
    progressData->colorTempList.emplace_back(_RGB(190, 196, 202));
    progressData->colorTempList.emplace_back(_RGB(255, 112, 36));
    progressData->colorTempList.emplace_back(_RGB(0, 192, 144));

    tpShared<tpCssData> normalCss = currentStatusCss();

    progressData->itemFont->setFontSize(normalCss->fontSize());
    progressData->itemFont->setFontColor(normalCss->color(), normalCss->color());

    data_ = progressData;

    refreshBaseCss();

    // CSS中的圆角应用于进度条，而不应用于整个窗体
    // setRoundCorners(0);
}

tpPercentProgressBar::~tpPercentProgressBar()
{
    tpPercentProgressBarData *progressData = static_cast<tpPercentProgressBarData *>(data_);
    if (progressData)
    {
        delete progressData;
        progressData = nullptr;
    }
}

void tpPercentProgressBar::setTotalValue(const double &totalValue)
{
    tpPercentProgressBarData *progressData = static_cast<tpPercentProgressBarData *>(data_);
    progressData->totalValue = totalValue;

    update();
}

uint32_t tpPercentProgressBar::addItem(const tpString &name, const double &value)
{
    tpPercentProgressBarData *progressData = static_cast<tpPercentProgressBarData *>(data_);

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

void tpPercentProgressBar::removeItem(const uint32_t &index)
{
    tpPercentProgressBarData *progressData = static_cast<tpPercentProgressBarData *>(data_);
    progressData->itemList.remove(index);
    update();
}

void tpPercentProgressBar::setItemText(const uint32_t &index, const tpString &text)
{
    tpPercentProgressBarData *progressData = static_cast<tpPercentProgressBarData *>(data_);

    if (index >= progressData->itemList.size())
        return;

    progressData->itemList[index].name = text;
    update();
}

void tpPercentProgressBar::setItemColor(const uint32_t &index, const int32_t &color)
{
    tpPercentProgressBarData *progressData = static_cast<tpPercentProgressBarData *>(data_);

    if (index >= progressData->itemList.size())
        return;

    progressData->itemList[index].color = color;
    update();
}

void tpPercentProgressBar::setItemValue(const uint32_t &index, const double &value)
{
    tpPercentProgressBarData *progressData = static_cast<tpPercentProgressBarData *>(data_);

    if (index >= progressData->itemList.size())
        return;

    progressData->itemList[index].value = value;
    update();
}

bool tpPercentProgressBar::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    tpPercentProgressBarData *progressData = static_cast<tpPercentProgressBarData *>(data_);
    if (progressData->totalValue == 0)
        return true;

    if (progressData->itemList.size() == 0)
        return true;

    tpCanvas *paintCanvas = event->canvas();
    tpShared<tpCssData> normalCss = currentStatusCss();

    // 绘制进度条底色
    uint32_t progressHeight = height() - progressData->itemFont->pixelHeight() - normalCss->gap();

    paintCanvas->roundedBox(0, 0, width(), progressHeight, normalCss->roundCorners(), normalCss->subColor());

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

        // 第一个要绘制一半圆角，一半方角
        if (i == 0)
        {
            paintCanvas->roundedBox(curItemX, 0, curItemX + curItemWidth, progressHeight, progressHeight / 2.0, itemInfo.color);

            // 绘制方角
            paintCanvas->box(curItemX + curItemWidth - progressHeight / 2.0, 0, curItemX + curItemWidth, progressHeight, itemInfo.color);
        }
        else if (i == (progressData->itemList.size() - 1))
        {
            if (std::fabs(curTotalValue - progressData->totalValue) < 1e-3)
            {
                // 最后一个如果刚好绘制到末尾，也要一半圆角，一半方角
                paintCanvas->roundedBox(curItemX, 0, curItemX + curItemWidth, progressHeight, progressHeight / 2.0, itemInfo.color);
            }
            else
            {
                paintCanvas->box(curItemX, 0, curItemX + curItemWidth, progressHeight, itemInfo.color);
            }
        }
        else
        {
            paintCanvas->box(curItemX, 0, curItemX + curItemWidth, progressHeight, itemInfo.color);
        }

        // 绘制item的提示文本
        progressData->itemFont->setText(itemInfo.name);
        paintCanvas->roundedBox(curItemContentX, itemContentY + 2, curItemContentX + itemContentCirrleWidth, height() - 2, itemContentCirrleWidth / 2.0, itemInfo.color);

        curItemContentX += itemContentCirrleWidth + 5;

        paintCanvas->renderText(*progressData->itemFont, curItemContentX, itemContentY);

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

bool tpPercentProgressBar::onResizeEvent(tpObjectResizeEvent *event)
{
    tpChildWidget::onResizeEvent(event);

    return true;
}
