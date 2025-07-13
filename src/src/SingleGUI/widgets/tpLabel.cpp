#include "tpLabel.h"
#include "tpCanvas.h"
#include "tpFont.h"
#include "tpEvent.h"
#include "tpRect.h"
#include "tpDisplay.h"

#include <cstring>
#include <iostream>

struct ItpTextLabelSet
{
	tpFont *font = nullptr;
	tinyPiX::AlignmentFlag align;
	bool enableFit;

	int textSpacing = 2;

	bool wrap = false;
};

tpVector<tpString> wrapText(ItpTextLabelSet *data, const tpString &text, const uint32_t &winWidth)
{
	tpVector<tpString> subStrList;

	int curStartIndex = 0;
	const int totalLength = text.logicalLength(); // 先获取总长度避免重复调用

	while (curStartIndex < totalLength)
	{
		int bestFit = 1; // 至少一个字符
		bool foundFit = false;

		// 从1个字符开始尝试，找到最长可容纳的子串
		for (int i = 1; (curStartIndex + i) <= totalLength; ++i)
		{
			tpString subStr = text.mid(curStartIndex, i);
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
			tpString subStrRes = text.mid(curStartIndex, bestFit);
			subStrList.emplace_back(subStrRes);
			curStartIndex += bestFit;
		}
		// 处理最后一段（未超宽且剩余文本）
		else if (bestFit > 0)
		{
			tpString subStrRes = text.mid(curStartIndex, bestFit);
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

tpLabel::tpLabel(tpChildWidget *parent)
	: tpChildWidget(parent)
{
	ItpTextLabelSet *set = new ItpTextLabelSet();
	this->textLabelSet = set;

	if (!set)
		return;

	set->font = new tpFont();

	if (set->font == nullptr)
	{
		std::cout << "font init error!" << std::endl;
	}

	set->align = tinyPiX::AlignLeft;
	set->enableFit = false;

	this->setEnableBackGroundImage(false);
	this->setEnableBackGroundColor(false);

	// refreshBaseCss();
}

tpLabel::tpLabel(const tpString &text, tpChildWidget *parent)
	: tpChildWidget(parent)
{
	ItpTextLabelSet *set = new ItpTextLabelSet();

	if (!set)
		return;

	set->font = new tpFont();

	if (set->font == nullptr)
	{
		std::cout << "font init error!" << std::endl;
	}

	set->align = tinyPiX::AlignLeft;
	set->enableFit = false;

	this->setEnableBackGroundImage(false);
	this->setEnableBackGroundColor(false);
	this->textLabelSet = set;

	setText(text);
}

tpLabel::~tpLabel()
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

void tpLabel::setAutoFit(bool enable)
{
	ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

	if (set)
	{
		set->enableFit = enable;
		if (enable)
		{
			ItpSize size = set->font->pixelSize();
			this->setRect(this->rect().x, this->rect().y, size.w, size.h);
		}
	}
}

void tpLabel::setText(const tpString &text)
{
	if (text.empty())
		return;

	tpChildWidget::setText(text);
	ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

	if (!set)
		return;

	set->font->setText(text);

	if (set->enableFit)
	{
		ItpSize size = set->font->pixelSize();
		this->setRect(this->rect().x, this->rect().y, size.w, size.h);
	}

	// 根据文本宽度调整最小宽度,只有没有设置固定宽度情况下才动态调整
	if (!isFixedWidth())
	{
		set->font->setText(this->text());

		if (set->wrap)
		{
			tpVector<tpString> subStrList = wrapText(set, this->text(), width());
			if (subStrList.size() != 0)
				setMinumumHeight(set->font->pixelHeight() * subStrList.size() + set->textSpacing * (subStrList.size() - 1));

			if (set->font->pixelWidth() > tpDisplay::dp2Px(131))
			{
				setMinumumWidth(tpDisplay::dp2Px(131));
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

void tpLabel::setWordWrap(bool wrap)
{
	ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

	set->wrap = wrap;
}

void tpLabel::setRect(tpRect &rect)
{
	this->setRect(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

void tpLabel::setRect(tpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->X0(), rect->Y0(), rect->width(), rect->height());
	}
}

void tpLabel::setRect(ItpRect &rect)
{
	this->setRect(rect.x, rect.y, rect.w, rect.h);
}

void tpLabel::setRect(ItpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->x, rect->y, rect->w, rect->h);
	}
}

void tpLabel::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
	ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

	if (set)
	{
		if (set->enableFit)
		{
			ItpSize size = set->font->pixelSize();
			tpChildWidget::setRect(x, y, size.w, size.h);
			return;
		}

		tpChildWidget::setRect(x, y, w, h);
	}
}

tpFont *tpLabel::font()
{
	ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;
	tpFont *font = nullptr;

	if (set)
	{
		font = set->font;
	}

	return font;
}

void tpLabel::setAlign(const tinyPiX::AlignmentFlag align)
{
	ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

	if (set)
	{
		set->align = align;
	}
}

bool tpLabel::onPaintEvent(tpObjectPaintEvent *event)
{
	ItpTextLabelSet *set = (ItpTextLabelSet *)this->textLabelSet;

	if (!set)
		return true;

	tpVector<tpString> subStrList = wrapText(set, text(), width());

	// 根据文本宽度调整最小宽度,只有没有设置固定宽度情况下才动态调整
	if (!isFixedWidth())
	{
		set->font->setText(text());

		if (set->wrap)
		{
			if (subStrList.size() != 0)
				setMinumumHeight(set->font->pixelHeight() * subStrList.size() + set->textSpacing * (subStrList.size() - 1));

			if (set->font->pixelWidth() > tpDisplay::dp2Px(131))
			{
				setMinumumWidth(tpDisplay::dp2Px(131));
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

	tpChildWidget::onPaintEvent(event);

	tpCanvas *canvas = event->canvas();
	tpString text = this->text();

	if (text.empty())
		return true;

	ItpSize size = set->font->pixelSize();
	int32_t cx = 0, cy = (int32_t)(event->rect().h - size.h) / 2;

	if (set->enableFit == false)
	{
		switch (set->align)
		{
		case tinyPiX::AlignLeft:
		{
			cx = 0;
		}
		break;
		case tinyPiX::AlignRight:
		{
			cx = (int32_t)(event->rect().w - size.w);
		}
		break;
		case tinyPiX::AlignHCenter:
		case tinyPiX::AlignCenter:
		{
			cx = (int32_t)(event->rect().w - size.w) / 2;
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
			canvas->renderText(*set->font, cx, cy);
		}
		else
		{
			// 重新计算起始Y坐标
			cy = (height() - (set->font->pixelHeight() * subStrList.size() + set->textSpacing * (subStrList.size() - 1))) / 2.0;

			for (int i = 0; i < subStrList.size(); ++i)
			{
				tpString subText = subStrList.at(i);
				set->font->setText(subText);

				ItpSize size = set->font->pixelSize();

				switch (set->align)
				{
				case tinyPiX::AlignLeft:
				{
					cx = 0;
				}
				break;
				case tinyPiX::AlignRight:
				{
					cx = (int32_t)(event->rect().w - size.w);
				}
				break;
				case tinyPiX::AlignHCenter:
				case tinyPiX::AlignCenter:
				{
					cx = (int32_t)(event->rect().w - size.w) / 2;
				}
				break;
				default:
					return false;
				}

				// std::cout << " subText " << subText << std::endl;
				canvas->renderText(*set->font, cx, cy + i * (size.h + set->textSpacing));
			}
		}
	}
	else
	{
		canvas->renderText(*set->font, cx, cy);
	}

	return true;
}

bool tpLabel::onLeaveEvent(tpObjectLeaveEvent *event)
{
	// std::cout << " tpLabel::onLeaveEvent " << event->leave() << std::endl;

	return true;
}
