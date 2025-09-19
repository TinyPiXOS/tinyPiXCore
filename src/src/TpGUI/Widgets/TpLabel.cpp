#include "TpLabel.h"
#include "TpPainter.h"
#include "TpFont.h"
#include "TpEvent.h"
#include "TpRect.h"
#include "TpDisplay.h"

#include <cstring>
#include <iostream>

struct ItpTextLabelSet
{
    TpFont *font = nullptr;
    Tp::AlignmentFlag align;
    bool enableFit;

    int textSpacing = 2;

    bool wrap = false;
};

TpVector<TpString> wrapText(ItpTextLabelSet *data, const TpString &text, const uint32_t &winWidth)
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

TpLabel::TpLabel(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    ItpTextLabelSet *set = new ItpTextLabelSet();
    this->textLabelSet = set;

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

TpLabel::TpLabel(const TpString &text, TpChildWidget *parent)
    : TpChildWidget(parent)
{
    ItpTextLabelSet *set = new ItpTextLabelSet();

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
    this->textLabelSet = set;

    setText(text);
}

TpLabel::~TpLabel()
{
    ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

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
    ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

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

    TpChildWidget::setText(text);
    ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

    if (!set)
        return;

    set->font->setText(text);

    if (set->enableFit)
    {
        TpSize size = set->font->pixelSize();
        this->setRect(this->rect().x(), this->rect().y(), size.width(), size.height());
    }

    // 根据文本宽度调整最小宽度,只有没有设置固定宽度情况下才动态调整
    if (!isFixedWidth())
    {
        set->font->setText(this->text());

        if (set->wrap)
        {
            TpVector<TpString> subStrList = wrapText(set, this->text(), width());
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
            setMinumumWidth(set->font->pixelWidth());
        }
    }
    if (!isFixedHeight())
    {
        setMinumumHeight(set->font->pixelHeight());
    }

    update();
}

void TpLabel::setWordWrap(bool wrap)
{
    ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

    set->wrap = wrap;
}

void TpLabel::setRect(const TpRect &rect)
{
    this->setRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void TpLabel::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

    if (set)
    {
        if (set->enableFit)
        {
            TpSize size = set->font->pixelSize();
            TpChildWidget::setRect(x, y, size.width(), size.height());
            return;
        }

        TpChildWidget::setRect(x, y, w, h);
    }
}

TpFont *TpLabel::font()
{
    ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;
    TpFont *font = nullptr;

    if (set)
    {
        font = set->font;
    }

    return font;
}

void TpLabel::setAlign(const Tp::AlignmentFlag align)
{
    ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

    if (set)
    {
        set->align = align;
    }
}

bool TpLabel::onPaintEvent(TpPaintEvent *event)
{
    ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;
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

    TpChildWidget::onPaintEvent(event);

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
