#include "TpLabel.h"
#include "TpPainter.h"
#include "TpFont.h"
#include "TpEvent.h"
#include "TpRect.h"
#include "SystemInfo/TpDisplay.h"

#include <cstring>
#include <iostream>

struct TpLabelData
{
    TpString text = "";

    TpFont *font = nullptr;
    Tp::AlignmentFlag align;
    bool enableFit;

    int textSpacing = 2;

    bool wrap = false;
};

TpVector<TpString> wrapText(TpLabelData *data, const TpString &text, const uint32_t &winWidth)
{
    TpVector<TpString> subStrList;

    int curStartIndex = 0;
    const int totalLength = text.logicalLength(); // 先获取总长度避免重复调用

    while (curStartIndex < totalLength)
    {
        int bestFit = 1; // 至少一个字符
        bool foundFit = false;

        // 从1个字符开始尝试，找到最长可容纳的子串
        for (int i = 1; (curStartIndex + i) <= totalLength; ++i)
        {
            TpString subStr = text.mid(curStartIndex, i);
            data->font->setText(subStr);

            if (data->font->pixelWidth() > winWidth)
            {
                // 找到最佳分割点（上一个长度）
                foundFit = true;
                break;
            }

            bestFit = i; // 更新最佳长度
        }

        // 处理找到的分割点
        if (foundFit && bestFit > 0)
        {
            TpString subStrRes = text.mid(curStartIndex, bestFit);
            subStrList.emplace_back(subStrRes);
            curStartIndex += bestFit;
        }
        // 处理最后一段（未超宽且剩余文本）
        else if (bestFit > 0)
        {
            TpString subStrRes = text.mid(curStartIndex, bestFit);
            subStrList.emplace_back(subStrRes);
            curStartIndex += bestFit; // 确保退出循环
        }
        else // 安全防护：防止死循环
        {
            curStartIndex = totalLength;
        }
    }

    return subStrList;
}

TpLabel::TpLabel(TpWidget *parent)
    : TpWidget(parent)
{
    TpLabelData *set = new TpLabelData();
    this->data_ = set;

    if (!set)
        return;

    set->font = new TpFont();

    if (set->font == nullptr)
    {
        std::cout << "font init error!" << std::endl;
    }

    set->align = Tp::AlignLeft;
    set->enableFit = false;

    this->setEnableBackGroundImage(false);
    this->setEnableBackGroundColor(false);

    // refreshBaseCss();
}

TpLabel::TpLabel(const TpString &text, TpWidget *parent)
    : TpWidget(parent)
{
    TpLabelData *set = new TpLabelData();

    if (!set)
        return;

    set->font = new TpFont();

    if (set->font == nullptr)
    {
        std::cout << "font init error!" << std::endl;
    }

    set->align = Tp::AlignLeft;
    set->enableFit = false;

    this->setEnableBackGroundImage(false);
    this->setEnableBackGroundColor(false);
    this->data_ = set;

    setText(text);
}

TpLabel::~TpLabel()
{
    TpLabelData *set = (TpLabelData *)this->data_;

    if (set)
    {
        if (set->font)
        {
            delete set->font;
        }

        delete set;
    }
}

void TpLabel::setAutoFit(bool enable)
{
    TpLabelData *set = (TpLabelData *)this->data_;

    if (set)
    {
        set->enableFit = enable;
        if (enable)
        {
            TpSize size = set->font->pixelSize();
            this->setRect(this->rect().x(), this->rect().y(), size.width(), size.height());
        }
    }
}

void TpLabel::setText(const TpString &text)
{
    if (text.empty())
        return;

    TpLabelData *labelData = static_cast<TpLabelData *>(data_);
    if (!labelData)
        return;

    labelData->text = text;
    labelData->font->setText(text);

    if (labelData->enableFit)
    {
        TpSize size = labelData->font->pixelSize();
        setRect(rect().x(), this->rect().y(), size.width(), size.height());
    }

    // 根据文本宽度调整最小宽度,只有没有设置固定宽度情况下才动态调整
    if (!isFixedWidth())
    {
        if (labelData->wrap)
        {
            TpVector<TpString> subStrList = wrapText(labelData, this->text(), width());
            if (subStrList.size() != 0)
                setMinumumHeight(labelData->font->pixelHeight() * subStrList.size() + labelData->textSpacing * (subStrList.size() - 1));

            if (labelData->font->pixelWidth() > TpDisplay::dp2Px(131))
            {
                setMinumumWidth(TpDisplay::dp2Px(131));
            }
            else
            {
                setMinumumWidth(labelData->font->pixelWidth());
            }
        }
        else
        {
            setMinumumWidth(labelData->font->pixelWidth());
        }
    }
    if (!isFixedHeight())
    {
        setMinumumHeight(labelData->font->pixelHeight());
    }

    update();
}

TpString TpLabel::text() const
{
    TpLabelData *labelData = static_cast<TpLabelData *>(data_);
    if (!labelData)
        return TpString();
    return labelData->text;
}

void TpLabel::setWordWrap(bool wrap)
{
    TpLabelData *set = (TpLabelData *)this->data_;

    set->wrap = wrap;
}

void TpLabel::setRect(const TpRect &rect)
{
    this->setRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void TpLabel::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    TpLabelData *set = (TpLabelData *)this->data_;

    if (set)
    {
        if (set->enableFit)
        {
            TpSize size = set->font->pixelSize();
            TpWidget::setRect(x, y, size.width(), size.height());
            return;
        }

        TpWidget::setRect(x, y, w, h);
    }
}

TpFont *TpLabel::font()
{
    TpLabelData *set = (TpLabelData *)this->data_;
    TpFont *font = nullptr;

    if (set)
    {
        font = set->font;
    }

    return font;
}

void TpLabel::setAlign(const Tp::AlignmentFlag align)
{
    TpLabelData *set = (TpLabelData *)this->data_;

    if (set)
    {
        set->align = align;
    }
}

bool TpLabel::onPaintEvent(TpPaintEvent *event)
{
    TpLabelData *set = (TpLabelData *)this->data_;
    if (!set)
        return true;

    // // 设置CSS
    // tpShared<TpCssData> curCssData = currentStatusCss();
    // set->font->setFontColor(curCssData->color(), curCssData->color());
    // set->font->setFontSize(curCssData->fontSize());

    TpVector<TpString> subStrList = wrapText(set, text(), width());

    // 根据文本宽度调整最小宽度,只有没有设置固定宽度情况下才动态调整
    if (!isFixedWidth())
    {
        set->font->setText(text());

        if (set->wrap)
        {
            if (subStrList.size() != 0)
                setMinumumHeight(set->font->pixelHeight() * subStrList.size() + set->textSpacing * (subStrList.size() - 1));

            if (set->font->pixelWidth() > TpDisplay::dp2Px(131))
            {
                setMinumumWidth(TpDisplay::dp2Px(131));
            }
            else
            {
                setMinumumWidth(set->font->pixelWidth());
            }
        }
        else
        {
            if (!text().empty())
                setMinumumWidth(set->font->pixelWidth());
        }
    }
    // 下边计算完行数，设置最小高度
    if (!isFixedHeight())
    {
        if (!text().empty())
            setMinumumHeight(set->font->pixelHeight());
    }

    TpWidget::onPaintEvent(event);

    TpPainter *canvas = event->painter();
    TpString text = this->text();

    if (text.empty())
        return true;

    TpSize size = set->font->pixelSize();
    int32_t cx = 0, cy = (event->rect().height() - size.height()) / 2;

    if (set->enableFit == false)
    {
        switch (set->align)
        {
        case Tp::AlignLeft:
        {
            cx = 0;
        }
        break;
        case Tp::AlignRight:
        {
            cx = event->rect().width() - size.width();
        }
        break;
        case Tp::AlignHCenter:
        case Tp::AlignCenter:
        {
            cx = (event->rect().width() - size.width()) / 2;
        }
        break;
        default:
            return false;
        }
    }

    if (set->wrap)
    {
        // 如果没有超过边界正常画就行
        if (subStrList.size() == 0)
        {
            set->font->setText(text);
            canvas->drawText(*set->font, cx, cy);
        }
        else
        {
            // 重新计算起始Y坐标
            cy = (height() - (set->font->pixelHeight() * subStrList.size() + set->textSpacing * (subStrList.size() - 1))) / 2.0;

            for (int i = 0; i < subStrList.size(); ++i)
            {
                TpString subText = subStrList.at(i);
                set->font->setText(subText);

                TpSize size = set->font->pixelSize();

                switch (set->align)
                {
                case Tp::AlignLeft:
                {
                    cx = 0;
                }
                break;
                case Tp::AlignRight:
                {
                    cx = event->rect().width() - size.width();
                }
                break;
                case Tp::AlignHCenter:
                case Tp::AlignCenter:
                {
                    cx = (event->rect().width() - size.width()) / 2;
                }
                break;
                default:
                    return false;
                }

                // std::cout << " subText " << subText << std::endl;
                canvas->drawText(*set->font, cx, cy + i * (size.height() + set->textSpacing));
            }
        }
    }
    else
    {
        canvas->drawText(*set->font, cx, cy);
    }

    return true;
}

bool TpLabel::onLeaveEvent(TpLeaveEvent *event)
{
    // std::cout << " TpLabel::onLeaveEvent " << event->leave() << std::endl;

    return true;
}
