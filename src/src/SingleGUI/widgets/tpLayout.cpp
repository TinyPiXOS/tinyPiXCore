#include "tpLayout.h"
#include "tpEvent.h"
#include "tpApp.h"
#include "tpRect.h"
#include "tpPoint.h"
#include "tpBoxLayout.h"
#include "tpLayoutItem.h"
#include "tpSignalSlot.h"

struct widgetItemData
{
	tpChildWidget *widget;

	tpLayout *layout;
	// 子布局的容器widget
	tpChildWidget *layoutWidget;

	tpSpacerItem *spacerItem;

	uint32_t strech;

	widgetItemData()
		: widget(nullptr), layout(nullptr), layoutWidget(nullptr), strech(1), spacerItem(nullptr)
	{
	}

	~widgetItemData()
	{
	}
};

struct tpLayoutData
{
	tpList<widgetItemData> widgetList;

	uint32_t totalStrech = 0;

	uint32_t leftMargin = 3;
	uint32_t topMargin = 3;
	uint32_t rightMargin = 3;
	uint32_t bottomMargin = 3;

	uint32_t spacing = 5;

	tpLayoutData()
	{
	}
};

// 计算项的布局方向有效策略
static tpSpacerItem::tpSizePolicy effectivePolicy(
	const widgetItemData &item,
	bool isHorizontal)
{
	if (!item.spacerItem)
		return tpSpacerItem::Expanding;

	return isHorizontal ? item.spacerItem->horizontalPolicy() : item.spacerItem->verticalPolicy();
}

// 获取项的基准尺寸
static ItpSize baseSize(const widgetItemData &item, bool isHorizontal)
{
	if (item.widget)
	{
		return ItpSize(item.widget->width(), item.widget->height());
	}
	if (item.spacerItem)
	{
		return item.spacerItem->sizeHint();
	}
	if (item.layoutWidget)
	{
		return ItpSize(item.layoutWidget->width(), item.layoutWidget->height());
	}
	return ItpSize();
}

tpLayout::tpLayout(tpChildWidget *parent)
	: tpObject(parent)
{
	if (parent)
	{
		parent->setLayout(this);
	}

	tpLayoutData *layoutData = new tpLayoutData();
	data_ = layoutData;
}

tpLayout::~tpLayout()
{
	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);
	if (layoutData)
	{
		for (auto &itemData : layoutData->widgetList)
		{
			// 如果是子widget，不处理，指针由外部管理
			if (itemData.widget)
			{
				itemData.widget->setParent(nullptr);
				continue;
			}

			if (itemData.layout)
			{
				itemData.layout->setParent(nullptr);
				continue;
			}

			if (itemData.layoutWidget)
			{
				itemData.layoutWidget->setParent(nullptr);
				itemData.layoutWidget->deleteLater();
			}

			if (itemData.spacerItem)
			{
				itemData.spacerItem->setParent(nullptr);
				delete itemData.spacerItem;
				itemData.spacerItem = nullptr;
			}
		}
		layoutData->widgetList.clear();

		delete layoutData;
		layoutData = nullptr;
		data_ = nullptr;
	}
}

void tpLayout::addWidget(tpChildWidget *widget, int stretch)
{
	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);

	widgetItemData curData;
	curData.widget = widget;
	curData.strech = stretch;

	if (curData.strech == 0)
		curData.strech = 1;

	layoutData->totalStrech += curData.strech;
	layoutData->widgetList.emplace_back(curData);

	update();
}

void tpLayout::addLayout(tpLayout *layout, int stretch)
{
	if (!layout)
		return;

	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);

	widgetItemData curData;
	curData.layout = layout;
	curData.strech = stretch;

	curData.layoutWidget = new tpChildWidget();
	curData.layoutWidget->setLayout(curData.layout);
	// curData.layoutWidget->setBackGroundColor(_RGB(0, 0, 0));

	// connect(curData.layout, onUpdate, [=](){update();});

	if (curData.strech == 0)
		curData.strech = 1;

	layoutData->totalStrech += curData.strech;
	layoutData->widgetList.emplace_back(curData);

	update();
}

void tpLayout::insertWidget(uint32_t index, tpChildWidget *widget, int stretch)
{
	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);

	widgetItemData curData;
	curData.widget = widget;
	curData.strech = stretch;

	if (curData.strech == 0)
		curData.strech = 1;

	layoutData->totalStrech += curData.strech;
	layoutData->widgetList.insertData(index, curData);

	update();
}

void tpLayout::insertLayout(uint32_t index, tpLayout *layout, int stretch)
{
	if (!layout)
		return;

	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);

	widgetItemData curData;
	curData.layout = layout;
	curData.strech = stretch;

	curData.layoutWidget = new tpChildWidget();
	curData.layoutWidget->setLayout(curData.layout);

	if (curData.strech == 0)
		curData.strech = 1;

	layoutData->totalStrech += curData.strech;
	layoutData->widgetList.insertData(index, curData);

	// curData.layoutWidget->setBackGroundColor(_RGB(0, 0, 0));

	update();
}

void tpLayout::removeWidget(tpChildWidget *widget)
{
	if (!widget)
		return;

	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);
	for (int i = 0; i < layoutData->widgetList.size(); ++i)
	{
		widgetItemData curData = layoutData->widgetList.at(i);
		if (curData.widget == widget)
		{
			curData.widget->setParent(nullptr);
			layoutData->totalStrech -= curData.strech;
			layoutData->widgetList.remove(i);

			update();
			break;
		}
	}
}

void tpLayout::removeLayout(tpLayout *layout)
{
	if (!layout)
		return;

	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);
	for (int i = 0; i < layoutData->widgetList.size(); ++i)
	{
		widgetItemData curData = layoutData->widgetList.at(i);
		if (curData.layout == layout)
		{
			tpList<tpObject *> layoutChildObjList = curData.layoutWidget->objectList();
			for (const auto &childObj : layoutChildObjList)
			{
				childObj->uninstallEventFilter();
				childObj->setParent(nullptr);
			}

			layout->setParent(nullptr);
			layout->uninstallEventFilter();

			curData.layoutWidget->deleteLater();
			curData.layoutWidget = nullptr;

			layoutData->totalStrech -= curData.strech;
			layoutData->widgetList.remove(i);

			update();
			break;
		}
	}
}

void tpLayout::addSpacer(tpSpacerItem *spacer)
{
	if (!spacer)
		return;

	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);

	widgetItemData curData;
	curData.spacerItem = spacer;

	layoutData->widgetList.emplace_back(curData);
}

void tpLayout::setContentsMargins(int32_t left, int32_t top, int32_t right, int32_t bottom)
{
	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);
	layoutData->leftMargin = left;
	layoutData->topMargin = top;
	layoutData->rightMargin = right;
	layoutData->bottomMargin = bottom;
}

void tpLayout::contentsMargins(int32_t *left, int32_t *top, int32_t *right, int32_t *bottom) const
{
	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);

	if (left)
		*left = layoutData->leftMargin;
	if (top)
		*top = layoutData->topMargin;
	if (right)
		*right = layoutData->rightMargin;
	if (bottom)
		*bottom = layoutData->bottomMargin;
}

void tpLayout::setSpacing(int spacing)
{
	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);
	layoutData->spacing = spacing;
}

int tpLayout::spacing() const
{
	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);
	return layoutData->spacing;
}

void tpLayout::update()
{
	tpBoxLayout *boxLayout = dynamic_cast<tpBoxLayout *>(this);
	if (!boxLayout)
		return;

	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);
	if (!layoutData || layoutData->widgetList.empty())
		return;

	// 获取父控件（通过tpChildWidget的parent()或存储的parent_）
	tpChildWidget *parentWidget = dynamic_cast<tpChildWidget *>(parent());
	if (!parentWidget)
		return;

	tpBoxLayout::Direction direction = boxLayout->direction();

	bool isHorizon = false;
	if (direction == tpBoxLayout::LeftToRight || direction == tpBoxLayout::RightToLeft)
		isHorizon = true;

	// 获取父控件尺寸和边距
	int parentWidth = parentWidget->width();
	int parentHeight = parentWidget->height();
	int left, top, right, bottom;
	contentsMargins(&left, &top, &right, &bottom);

	// 计算可用区域
	int availableWidth = std::max(0, parentWidth - left - right);
	int availableHeight = std::max(0, parentHeight - top - bottom);
	int totalSpacing = (layoutData->widgetList.size() - 1) * layoutData->spacing;

	// 实际可用区域大小
	uint32_t remainingWidth = std::max(0, availableWidth - totalSpacing);
	uint32_t remainingHeight = std::max(0, availableHeight - totalSpacing);

	// 按拉伸因子分配高度
	int32_t currentX = left;
	int32_t currentY = top;

	uint32_t totalStretch = layoutData->totalStrech;

	// 是否是正序，从左到右，从上到下
	bool isOrder = true;
	if (direction == tpBoxLayout::RightToLeft)
	{
		isOrder = false;
		currentX = parentWidth - right;
	}
	else if (direction == tpBoxLayout::BottomToTop)
	{
		isOrder = false;
		currentY = parentHeight - bottom;
	}
	else
	{
	}

	// 计算固定项空间和有效拉伸因子 控件有最小宽度高度时，该行/列宽高不应小于控件最小宽高
	uint32_t fixedSpace = 0;
	uint32_t effectiveStretch = 0;

	// 所有有最小宽高的组件中，最大的最小宽高值
	uint32_t maxMinWidth = 0;
	uint32_t maxMinHeight = 0;

	// 弹簧的站位
	uint32_t expandingSpacerItemWidth = 0;
	uint32_t expandingSpacerItemHeight = 0;
	bool isHorizonExpanding = false;
	bool isVerticalExpanding = false;

	// 弹簧数量
	uint32_t spacerCount = 0;

	// 所有组件的最小宽度/高度之和，当有expanding弹簧时，确定弹簧的宽高
	uint32_t allWidgetMinWidth = 0;
	uint32_t allWidgetMinHeight = 0;

	// 如果水平/垂直布局，所有组件都是固定大小，则水平模式下，父窗口固定高度，垂直模式，固定宽度
	bool isFixedWidth = true;
	bool isFixedHeight = true;
	uint32_t maxFixedWidth = 0;
	uint32_t maxFixedHeight = 0;

	for (const auto &item : layoutData->widgetList)
	{
		tpChildWidget *operatorWidget = nullptr;
		if (item.widget)
		{
			operatorWidget = item.widget;
		}
		else if (item.layout)
		{
			operatorWidget = item.layoutWidget;
		}
		else if (item.spacerItem)
		{
			if (item.spacerItem->horizontalPolicy() == tpSpacerItem::Expanding)
				isHorizonExpanding = true;

			if (item.spacerItem->verticalPolicy() == tpSpacerItem::Expanding)
				isVerticalExpanding = true;
			// item.spacerItem->sizeHint();

			spacerCount++;
			ItpSize spacerItemSize = item.spacerItem->sizeHint();

			if (isHorizon)
			{
				if (spacerItemSize.h > maxMinHeight)
					maxMinHeight = spacerItemSize.h;
			}
			else
			{
				if (spacerItemSize.w > maxMinWidth)
					maxMinWidth = spacerItemSize.w;
			}
		}
		else
		{
		}

		if (!operatorWidget)
			continue;

		// 控件不显示，则不处理计算逻辑
		if (!operatorWidget->visible())
		{
			// 控件隐藏了，所以要把扣除的spacing在添加回来
			remainingWidth += layoutData->spacing;
			remainingHeight += layoutData->spacing;

			continue;
		}

		// 累加布局方向上的固定尺寸
		if (isHorizon)
		{
			if (operatorWidget->isFixedWidth())
			{
				fixedSpace += operatorWidget->width();
			}
			else
			{
				effectiveStretch += item.strech;
			}

			if (operatorWidget->isFixedHeight())
			{
				if (operatorWidget->minumumHeight() > maxFixedHeight)
					maxFixedHeight = operatorWidget->minumumHeight();
			}
			else
			{
				isFixedHeight = false;
			}
		}
		else
		{
			if (operatorWidget->isFixedHeight())
			{
				fixedSpace += operatorWidget->height();
			}
			else
			{
				effectiveStretch += item.strech;
			}

			if (operatorWidget->isFixedWidth())
			{
				if (operatorWidget->minumumWidth() > maxFixedWidth)
					maxFixedWidth = operatorWidget->minumumWidth();
			}
			else
			{
				isFixedWidth = false;
			}
		}

		if (operatorWidget->minumumHeight() > maxMinHeight)
			maxMinHeight = operatorWidget->minumumHeight();

		if (operatorWidget->minumumWidth() > maxMinWidth)
			maxMinWidth = operatorWidget->minumumWidth();

		allWidgetMinWidth += operatorWidget->minumumWidth();
		allWidgetMinHeight += operatorWidget->minumumHeight();
	}

	// 根据子窗体的最小宽高设置父窗体的最小宽高
	if (isHorizon)
	{
		if (isFixedHeight)
		{
			uint32_t minWidth = allWidgetMinWidth + totalSpacing + left + right;
			parentWidget->setMinumumWidth(minWidth);

			parentWidget->setFixedHeight(maxFixedHeight + top + bottom);
		}
		else
		{
			// 如果已经设置过最小大小了，以数值大的为准
			uint32_t minWidth = allWidgetMinWidth + totalSpacing + left + right;
			uint32_t minHeight = maxMinHeight + top + bottom;

			// 0605 新增；外部没有设置过最小尺寸；布局才调整最小尺寸
			if (parentWidget->minumumWidth() == 0)
				parentWidget->setMinumumWidth(minWidth);

			if (parentWidget->minumumHeight() == 0)
				parentWidget->setMinumumHeight(minHeight);
		}
	}
	else
	{
		if (isFixedWidth)
		{
			uint32_t minHeight = allWidgetMinHeight + totalSpacing + top + bottom;
			parentWidget->setMinumumHeight(minHeight);

			parentWidget->setFixedWidth(maxFixedWidth + left + right);
		}
		else
		{
			// 如果已经设置过最小大小了，以数值大的为准
			uint32_t minWidth = maxMinWidth + left + right;
			uint32_t minHeight = allWidgetMinHeight + totalSpacing + top + bottom;

			if (parentWidget->minumumWidth() == 0)
				parentWidget->setMinumumWidth(minWidth);

			// if (minHeight > parentWidget->minumumHeight())
			// parentWidget->setMinumumHeight(minHeight);

			if (parentWidget->minumumHeight() == 0)
				parentWidget->setMinumumHeight(minHeight);
		}
	}

	expandingSpacerItemWidth = remainingWidth - allWidgetMinWidth;
	expandingSpacerItemHeight = remainingWidth - allWidgetMinHeight;

	// 调整剩余可用空间（已扣除总间距和固定项）
	if (isHorizon)
	{
		if (spacerCount != 0)
			expandingSpacerItemWidth /= spacerCount;

		remainingWidth = std::max((uint32_t)0, remainingWidth - fixedSpace);
	}
	else
	{
		if (spacerCount != 0)
			expandingSpacerItemHeight /= spacerCount;

		remainingHeight = std::max((uint32_t)0, remainingHeight - fixedSpace);
	}

	for (int i = 0; i < layoutData->widgetList.size(); ++i)
	{
		const auto &item = layoutData->widgetList[i];

		// 计算子控件高度/宽度
		uint32_t itemWidth = isHorizon ? ((1.0 * item.strech / totalStretch) * remainingWidth) : availableWidth;
		uint32_t itemHeight = isHorizon ? availableHeight : ((1.0 * item.strech / totalStretch) * remainingHeight);

		tpChildWidget *operatorWidget = nullptr;
		if (item.widget)
		{
			operatorWidget = item.widget;
		}
		else if (item.layout)
		{
			operatorWidget = item.layoutWidget;
		}
		else if (item.spacerItem)
		{
			// 如果是弹簧，坐标偏移弹簧的宽高
			if (isHorizon)
			{
				if (isHorizonExpanding)
				{
					currentX += expandingSpacerItemWidth + layoutData->spacing;
				}
				else
				{
					if (isOrder)
						currentX += item.spacerItem->sizeHint().w + layoutData->spacing;
				}
			}
			else
			{
				if (isVerticalExpanding)
				{
					currentY += expandingSpacerItemHeight + layoutData->spacing;
				}
				else
				{
					if (isOrder)
						currentY += item.spacerItem->sizeHint().h + layoutData->spacing;
				}
			}

			continue;
		}
		else
		{
			continue;
		}

		// 设置控件位置和尺寸
		if (operatorWidget->parent() != parentWidget)
		{
			if (item.layout)
			{
				operatorWidget->installEventFilter(parentWidget);
				item.layout->update();
			}
			operatorWidget->setParent(parentWidget);
		}

		// 控件不显示，则不处理计算逻辑
		if (!operatorWidget->visible())
			continue;

		// 判断是否是固定项
		bool isFixed = false;
		if (isHorizon)
		{
			isFixed = operatorWidget->isFixedWidth();
		}
		else
		{
			isFixed = operatorWidget->isFixedHeight();
		}

		if (isFixed)
		{
			// 固定项直接使用预设尺寸
			if (isHorizon)
			{
				itemWidth = operatorWidget->width();

				if (operatorWidget->isFixedHeight())
				{
					itemHeight = operatorWidget->height();
				}
				else
				{
					// 垂直方向填满
					itemHeight = availableHeight;
				}
			}
			else
			{
				itemHeight = operatorWidget->height();

				if (operatorWidget->isFixedWidth())
				{
					itemWidth = operatorWidget->width();
				}
				else
				{
					itemWidth = availableWidth; // 水平方向填满
				}
			}
		}
		else
		{
			// 部件非固定尺寸，如果有水平完全伸展弹簧，则所有同层级组件使用最小尺寸
			if (isHorizonExpanding || isVerticalExpanding)
			{
				if (isHorizonExpanding && isHorizon)
				{
					if (isHorizon)
					{
						itemWidth = operatorWidget->minumumWidth();
						itemHeight = availableHeight;
					}
				}

				if (isVerticalExpanding && (!isHorizon))
				{
					itemWidth = availableWidth;
					itemHeight = operatorWidget->minumumHeight();
				}
			}
			else
			{
				if (effectiveStretch > 0)
				{
					// 可伸展项按比例分配
					if (isHorizon)
					{
						itemWidth = (item.strech * remainingWidth) / effectiveStretch;
						itemHeight = availableHeight;
					}
					else
					{
						itemWidth = availableWidth;
						itemHeight = (item.strech * remainingHeight) / effectiveStretch;
					}
				}
			}
		}

		if (direction == tpBoxLayout::RightToLeft)
		{
			if (i == 0)
				currentX -= itemWidth;
			else
				currentX -= (itemWidth + layoutData->spacing);
		}
		else if (direction == tpBoxLayout::BottomToTop)
		{
			if (i == 0)
				currentY -= itemHeight;
			else
				currentY -= (itemHeight + layoutData->spacing);
		}
		else
		{
		}

		//  最后一个控件，缩放填充整个layout
		if (i == (layoutData->widgetList.size() - 1))
		{
			if (isHorizon)
			{
				if (isOrder)
					itemWidth = parentWidth - right - currentX;
				else
					currentX = left;
			}
			else
			{
				if (isOrder)
					itemHeight = parentHeight - bottom - currentY;
				else
					currentY = top;
			}
		}
		// std::cout << "button " << currentX << " " << currentY << " " << itemWidth << " " << itemHeight << std::endl;

		int xPos = currentX;
		int yPos = currentY;
		if (isHorizon)
		{
			if (operatorWidget->isFixedHeight() && itemHeight < availableHeight)
			{
				yPos = top + (availableHeight - itemHeight) / 2;
			}
		}
		else
		{
			if (operatorWidget->isFixedWidth() && itemWidth < availableHeight)
			{
				xPos = left + (availableWidth - itemWidth) / 2;
			}
		}

		// 如果是子布局；水平布局则子布局高度等于本窗体；垂直布局则宽度等于本窗体去掉margin
		// if (item.layout)
		// {
		// 	if (isHorizon)
		// 	{
		// 		itemHeight = parentHeight - top - bottom;
		// 		yPos = top + (availableHeight - itemHeight) / 2;
		// 	}
		// 	else
		// 	{
		// 		itemWidth = parentWidth - left - right;
		// 		xPos = left + (availableWidth - itemWidth) / 2;
		// 	}
		// }

		operatorWidget->setRect(xPos, yPos, itemWidth, itemHeight);

		/*  更新坐标（最后一个控件后不加间距）
			因为operatorWidget可能有最小宽高，比itemWidth/itemHeight大
			所以itemWidth/itemHeight不一定是widget的真实宽高*/
		if (isHorizon)
		{
			if (isOrder)
				currentX += operatorWidget->width() + layoutData->spacing;
		}
		else
		{
			if (isOrder)
				currentY += operatorWidget->height() + layoutData->spacing;
		}
	}
}

uint32_t tpLayout::rowCount()
{
	return 0;
}

uint32_t tpLayout::columnCount()
{
	return 0;
}

void tpLayout::clear()
{
	tpBoxLayout *boxLayout = dynamic_cast<tpBoxLayout *>(this);
	if (!boxLayout)
		return;

	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);
	if (!layoutData || layoutData->widgetList.empty())
		return;

	for (auto &item : layoutData->widgetList)
	{
		if (item.layoutWidget)
		{
			tpList<tpObject *> layoutChildObjList = item.layoutWidget->objectList();
			for (const auto &childObj : layoutChildObjList)
			{
				childObj->setParent(nullptr);
			}

			item.layoutWidget->deleteLater();
		}
		else if (item.widget)
		{
			item.widget->setParent(nullptr);
		}
		else if (item.spacerItem)
		{
			delete item.spacerItem;
			item.spacerItem = nullptr;
		}
		else
		{
		}
	}

	layoutData->widgetList.clear();
}

tpVector<tpObject*> tpLayout::children()
{
	tpVector<tpObject *> childList;

	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);
	if (!layoutData || layoutData->widgetList.empty())
		return childList;

	for (const auto &childData : layoutData->widgetList)
	{
		if (childData.widget)
		{
			childList.emplace_back(childData.widget);
		}
		else if (childData.layout)
		{
			childList.emplace_back(childData.layout);
		}
		else if (childData.spacerItem)
		{
			childList.emplace_back(childData.spacerItem);
		}
		else
		{
		}
	}

	return childList;
}

ItpSize tpLayout::minumumSize()
{
	ItpSize miniSize;

	tpBoxLayout *boxLayout = dynamic_cast<tpBoxLayout *>(this);
	if (!boxLayout)
		return miniSize;

	tpLayoutData *layoutData = static_cast<tpLayoutData *>(data_);
	if (!layoutData || layoutData->widgetList.empty())
		return miniSize;

	// 获取父控件（通过tpChildWidget的parent()或存储的parent_）
	tpChildWidget *parentWidget = dynamic_cast<tpChildWidget *>(parent());
	if (!parentWidget)
		return miniSize;

	tpBoxLayout::Direction direction = boxLayout->direction();

	bool isHorizon = false;
	if (direction == tpBoxLayout::LeftToRight || direction == tpBoxLayout::RightToLeft)
		isHorizon = true;

	// 获取父控件尺寸和边距
	int left, top, right, bottom;
	contentsMargins(&left, &top, &right, &bottom);

	int totalSpacing = (layoutData->widgetList.size() - 1) * layoutData->spacing;

	// 所有有最小宽高的组件中，最大的最小宽高值
	uint32_t maxMinWidth = 0;
	uint32_t maxMinHeight = 0;

	// 所有组件的最小宽度/高度之和，当有expanding弹簧时，确定弹簧的宽高
	uint32_t allWidgetMinWidth = 0;
	uint32_t allWidgetMinHeight = 0;

	// 如果水平/垂直布局，所有组件都是固定大小，则水平模式下，父窗口固定高度，垂直模式，固定宽度
	bool isFixedWidth = true;
	bool isFixedHeight = true;
	uint32_t maxFixedWidth = 0;
	uint32_t maxFixedHeight = 0;

	for (const auto &item : layoutData->widgetList)
	{
		tpChildWidget *operatorWidget = nullptr;
		if (item.widget)
		{
			operatorWidget = item.widget;
		}
		else if (item.layout)
		{
			operatorWidget = item.layoutWidget;
		}
		else if (item.spacerItem)
		{
		}
		else
		{
		}

		if (!operatorWidget)
			continue;

		// 累加布局方向上的固定尺寸
		if (isHorizon)
		{
			if (operatorWidget->isFixedHeight())
			{
				if (operatorWidget->minumumHeight() > maxFixedHeight)
					maxFixedHeight = operatorWidget->minumumHeight();
			}
			else
			{
				isFixedHeight = false;
			}
		}
		else
		{
			if (operatorWidget->isFixedWidth())
			{
				if (operatorWidget->minumumWidth() > maxFixedWidth)
					maxFixedWidth = operatorWidget->minumumWidth();
			}
			else
			{
				isFixedWidth = false;
			}
		}

		if (operatorWidget->minumumHeight() > maxMinHeight)
			maxMinHeight = operatorWidget->minumumHeight();

		if (operatorWidget->minumumWidth() > maxMinWidth)
			maxMinWidth = operatorWidget->minumumWidth();

		allWidgetMinWidth += operatorWidget->minumumWidth();
		allWidgetMinHeight += operatorWidget->minumumHeight();
	}

	// 根据子窗体的最小宽高设置父窗体的最小宽高
	if (isHorizon)
	{
		if (isFixedHeight)
		{
			uint32_t minWidth = allWidgetMinWidth + totalSpacing + left + right;

			miniSize.w = minWidth;
			miniSize.h = maxFixedHeight + top + bottom;
		}
		else
		{
			// 如果已经设置过最小大小了，以数值大的为准
			uint32_t minWidth = allWidgetMinWidth + totalSpacing + left + right;
			uint32_t minHeight = maxMinHeight + top + bottom;

			miniSize.w = minWidth;
			miniSize.h = minHeight;
		}
	}
	else
	{
		if (isFixedWidth)
		{
			uint32_t minHeight = allWidgetMinHeight + totalSpacing + top + bottom;

			miniSize.w = maxFixedWidth + left + right;
			miniSize.h = minHeight;
		}
		else
		{
			// 如果已经设置过最小大小了，以数值大的为准
			uint32_t minWidth = maxMinWidth + left + right;
			uint32_t minHeight = allWidgetMinHeight + totalSpacing + top + bottom;

			miniSize.w = minWidth;
			miniSize.h = minHeight;
		}
	}

	return miniSize;
}
