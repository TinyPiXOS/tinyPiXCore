#include "tpScrollPanel.h"
#include "tpScroll.h"
#include "tpEvent.h"
#include "tpCanvas.h"
#include "tpRect.h"
#include "tpMap.h"
#include "tpVector.h"
#include <climits>

#define TP_SCROLL_NUM 2
#define TP_LTSCR_INDEX_H 0
#define TP_RBSCR_INDEX_V 1

#ifndef WHEEL_SCROLL_INTERVAL
#define WHEEL_SCROLL_INTERVAL 10
#endif

struct tpScrollPanelData
{
	tpScroll *scroll[TP_SCROLL_NUM];
	tpChildWidget *panel;
	tpVector<tpChildWidget *> objectList;
	tpMap<tpChildWidget *, ItpRect> objectRect;

	// 滚动窗口
	tpChildWidget *centralWidget = nullptr;

	// 水平和垂直滚动条是否显示
	bool HScrollBarVisible = true;
	bool VScrollBarVisible = true;

	// 滚轮滚动模式,true为垂直滚动
	bool isVertical = true;

	ItpPoint ltPoint;
	ItpPoint rbPoint;

	int32_t offsetX;
	int32_t offsetY;

	// 鼠标左键是否按下
	bool mouseLeftPress = false;
	// 鼠标点击坐标
	ItpPoint originPressPoint_;
	// 鼠标拖拽更新坐标
	ItpPoint updatePoint_;
};

tpScrollPanel::tpScrollPanel(tpChildWidget *parent)
	: tpChildWidget(parent)
{
	tpScrollPanelData *set = new tpScrollPanelData();

	if (!set)
		return;

	set->panel = new tpChildWidget(this);
	set->panel->setEnableBackGroundColor(true);
	set->panel->setEnableBackGroundImage(false);
	set->panel->setEnabledBorderColor(false);
	set->panel->setVisible(true);

	set->panel->installEventFilter(this);
	// set->panel->setBackGroundColor(_RGB(0, 255, 0));
	for (int32_t i = 0; i < TP_SCROLL_NUM; i++)
	{
		set->scroll[i] = new tpScroll(this, i);
		set->scroll[i]->setVisible(false);
	}

	set->HScrollBarVisible = false;
	set->VScrollBarVisible = false;

	set->ltPoint.x = INT_MAX;
	set->rbPoint.y = INT_MAX;

	set->rbPoint.x = INT_MIN;
	set->rbPoint.y = INT_MIN;

	set->offsetX = 0;
	set->offsetY = 0;

	// connect(set->scroll[TP_LTSCR_INDEX_H], SIGNALS(tpScroll, onScroll, tpScroll *, int32_t, int32_t, int32_t, double), this, SLOTS(tpScrollPanel, doFlip, tpScroll *, int32_t, int32_t, int32_t, double));
	// connect(set->scroll[TP_RBSCR_INDEX_V], SIGNALS(tpScroll, onScroll, tpScroll *, int32_t, int32_t, int32_t, double), this, SLOTS(tpScrollPanel, doFlip, tpScroll *, int32_t, int32_t, int32_t, double));

	data_ = set;

	setVisible(true);
}

tpScrollPanel::~tpScrollPanel()
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);

	if (!set)
		return;

	if (set->panel)
	{
		delete set->panel;
	}

	int32_t i;

	for (i = 0; i < TP_SCROLL_NUM; i++)
	{
		if (set->scroll[i])
		{
			delete set->scroll[i];
		}
	}

	delete set;
}

uint32_t tpScrollPanel::horizontalScrollBarValue()
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return 0;

	tpScroll *scrollBar = set->scroll[TP_LTSCR_INDEX_H];
	uint32_t deltaPos = scrollBar->max() - scrollBar->min();
	if (deltaPos == 0)
		return 0;

	uint32_t finalPercent = 100 * (scrollBar->position() - scrollBar->min()) / deltaPos;

	return finalPercent;
}

void tpScrollPanel::setHorizontalScrollBarValue(const uint32_t &value)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return;

	tpScroll *scrollBar = set->scroll[TP_LTSCR_INDEX_H];

	uint32_t deltaPos = scrollBar->max() - scrollBar->min();
	uint32_t newPosion = value * deltaPos / 100.0;

	scrollBar->setPosition(newPosion);

	// std::cout << "newPosion : " << newPosion << std::endl;

	if (set->centralWidget)
	{
		int32_t x = set->centralWidget->pos().x;
		int32_t y = set->centralWidget->pos().y;

		set->centralWidget->move(-newPosion, y);
		set->centralWidget->broadSetTop();
	}
	else
	{
		int32_t x = set->panel->pos().x;
		int32_t y = set->panel->pos().y;

		set->panel->move(-newPosion, y);
		set->panel->broadSetTop();
	}

	tpChildWidget::update();
}

uint32_t tpScrollPanel::verticalScrollBarValue()
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return 0;

	tpScroll *scrollBar = set->scroll[TP_RBSCR_INDEX_V];
	uint32_t deltaPos = scrollBar->max() - scrollBar->min();

	if (deltaPos == 0)
		return 0;

	uint32_t finalPercent = 100 * (scrollBar->position() - scrollBar->min()) / deltaPos;

	return finalPercent;
}

void tpScrollPanel::setVerticalScrollBarValue(const uint32_t &value)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return;

	tpScroll *scrollBar = set->scroll[TP_RBSCR_INDEX_V];

	uint32_t deltaPos = scrollBar->max() - scrollBar->min();
	uint32_t newPosion = value * deltaPos / 100.0;

	scrollBar->setPosition(newPosion);

	if (set->centralWidget)
	{
		// std::cout << "set->centralWidget Height  " << set->centralWidget->height() << std::endl;

		int32_t x = set->centralWidget->pos().x;
		int32_t y = set->centralWidget->pos().y;

		set->centralWidget->move(x, -newPosion);
		set->centralWidget->broadSetTop();
	}
	else
	{
		int32_t x = set->panel->pos().x;
		int32_t y = set->panel->pos().y;

		set->panel->move(x, -newPosion);
		set->panel->broadSetTop();
	}

	tpChildWidget::update();
}

int32_t tpScrollPanel::horizontalPostion()
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return 0;

	if (set->centralWidget)
		return set->centralWidget->pos().x;

	return set->panel->pos().x;
}

void tpScrollPanel::setHorizontalPostion(const int32_t &value)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return;

	tpScroll *scrollBar = set->scroll[TP_LTSCR_INDEX_H];
	scrollBar->setPosition(-value);

	if (set->centralWidget)
	{
		int32_t x = set->centralWidget->pos().x;
		int32_t y = set->centralWidget->pos().y;

		set->centralWidget->move(value, y);
		set->centralWidget->broadSetTop();
	}
	else
	{
		int32_t x = set->panel->pos().x;
		int32_t y = set->panel->pos().y;

		set->panel->move(value, y);
		set->panel->broadSetTop();
	}

	tpChildWidget::update();
}

int32_t tpScrollPanel::verticalPostion()
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return 0;

	if (set->centralWidget)
		return set->centralWidget->pos().y;

	return set->panel->pos().y;
}

void tpScrollPanel::setVerticalPostion(const int32_t &value)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return;

	tpScroll *scrollBar = set->scroll[TP_RBSCR_INDEX_V];
	scrollBar->setPosition(-value);

	if (set->centralWidget)
	{
		int32_t x = set->centralWidget->pos().x;
		int32_t y = set->centralWidget->pos().y;

		set->centralWidget->move(x, value);
		set->centralWidget->broadSetTop();
	}
	else
	{
		int32_t x = set->panel->pos().x;
		int32_t y = set->panel->pos().y;

		set->panel->move(x, value);
		set->panel->broadSetTop();
	}

	tpChildWidget::update();
}

bool tpScrollPanel::scrollMode()
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return false;

	return set->isVertical;
}

void tpScrollPanel::setScrollMode(const bool &isVertical)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return;

	set->isVertical = isVertical;
}

void tpScrollPanel::setVerticalScrollBarVisible(const bool &visible)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return;

	set->scroll[TP_RBSCR_INDEX_V]->setVisible(visible);
	set->VScrollBarVisible = visible;
}

void tpScrollPanel::setHorizontalScrollBarVisible(const bool &visible)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return;

	set->scroll[TP_LTSCR_INDEX_H]->setVisible(visible);
	set->HScrollBarVisible = visible;
}

bool tpScrollPanel::addObject(tpChildWidget *object)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	bool ret = false;

	if (!set)
		return ret;

	object->setParent(set->panel);
	object->installEventFilter(this);
	ret = (object->parent() == set->panel);

	if (ret)
	{
		set->objectList.push_back(object);
	}

	recal();

	return ret;
}

bool tpScrollPanel::delObject(tpChildWidget *object)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	bool ret = false;

	if (!set)
		return ret;

	object->setParent(nullptr);
	object->uninstallEventFilter();
	ret = (object->parent() == nullptr);

	if (!ret)
		return ret;

	auto iter = set->objectList.begin();

	for (; iter != set->objectList.end(); iter++)
	{
		if (*iter == object)
		{
			break;
		}
	}

	if (iter != set->objectList.end())
	{
		set->objectList.erase(iter);
	}

	ItpRect rect = set->objectRect[object];
	object->setRect(&rect);

	auto mapIter = set->objectRect.find(object);

	if (mapIter != set->objectRect.end())
	{
		set->objectRect.erase(mapIter);
	}

	return ret;
}

bool tpScrollPanel::setWidget(tpChildWidget *widget)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return false;

	if (set->centralWidget)
	{
		set->centralWidget->uninstallEventFilter();
		set->centralWidget->setParent(nullptr);
	}

	set->panel->setBackGroundColor(_RGB(0, 255, 0));

	// addObject(widget);
	set->centralWidget = widget;

	if (widget)
	{
		widget->installEventFilter(this);
		widget->setParent(this);

		// widget->setRect(0, 0, set->panel->width(), set->panel->height());
		widget->setRect(0, 0, width(), height());
		recal();

		// std::cout << "SetWidget  :: set->centralWidget Height  " << set->centralWidget->height() << std::endl;
		// std::cout << "SetWidget  :: Height  " << height() << std::endl;
	}

	update();

	return true;
}

tpChildWidget *tpScrollPanel::widget()
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	return set->centralWidget;
}

tpVector<tpChildWidget *> tpScrollPanel::children()
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	return set->objectList;
}

bool tpScrollPanel::clearObject()
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);

	if (!set)
		return false;

	for (auto iter = set->objectList.begin(); iter != set->objectList.end(); iter++)
	{
		ItpRect rect = set->objectRect[*iter];
		(*iter)->setRect(&rect);

		auto mapIter = set->objectRect.find(*iter);

		if (mapIter != set->objectRect.end())
		{
			set->objectRect.erase(mapIter);
		}
	}

	set->objectList.clear();
	recal();

	return true;
}

bool tpScrollPanel::recal(bool enableOffset)
{
	// std::cout << " tpScrollPanel::recal(bool enableOffset) " << std::endl;
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	bool ret = false;

	if (!set)
		return false;

	uint32_t rectWidth = width();
	uint32_t rectHeight = height();

	if (set->centralWidget)
	{
		uint32_t realWidth = set->centralWidget->width();
		uint32_t realHeight = set->centralWidget->height();

		if (realWidth < rectWidth)
			realWidth = rectWidth;

		if (realHeight < rectHeight)
			realHeight = rectHeight;

		int32_t scrollAreaX = realWidth - rectWidth;
		int32_t scrollAreaY = realHeight - rectHeight;

		if (realWidth >= rectWidth)
		{
			set->scroll[TP_LTSCR_INDEX_H]->setRect(0, this->height() - 5, this->width() - 5, 5);
			set->scroll[TP_LTSCR_INDEX_H]->setMaxRange(scrollAreaX);
			set->scroll[TP_LTSCR_INDEX_H]->setLinePerPage(scrollAreaX / 10);
			set->scroll[TP_LTSCR_INDEX_H]->setPosition(0);
			set->scroll[TP_LTSCR_INDEX_H]->setVisible(set->HScrollBarVisible);
			realWidth -= 5;
		}
		else
		{
			set->scroll[TP_LTSCR_INDEX_H]->setVisible(false);
		}

		if (realHeight >= rectHeight)
		{
			set->scroll[TP_RBSCR_INDEX_V]->setRect(this->width() - 5, 0, 5, this->height() - 5);
			set->scroll[TP_RBSCR_INDEX_V]->setMaxRange(scrollAreaY);
			set->scroll[TP_RBSCR_INDEX_V]->setLinePerPage(scrollAreaY / 10);
			set->scroll[TP_RBSCR_INDEX_V]->setPosition(0);
			set->scroll[TP_RBSCR_INDEX_V]->setVisible(set->VScrollBarVisible);
			realHeight -= 5;
		}
		else
		{
			set->scroll[TP_RBSCR_INDEX_V]->setVisible(false);
		}
	}
	else
	{
		int32_t size = set->objectList.size();

		if (size == 0)
			return false;

		ItpPoint tltPoint, trbPoint;

		tltPoint.x = set->objectList.at(0)->rect().x;
		tltPoint.y = set->objectList.at(0)->rect().y;

		trbPoint.x = set->objectList.at(0)->rect().x + set->objectList.at(0)->rect().w;
		trbPoint.y = set->objectList.at(0)->rect().y + set->objectList.at(0)->rect().h;

		for (int32_t i = 1; i < size; i++)
		{
			tltPoint.x = TP_MIN(tltPoint.x, set->objectList.at(i)->pos().x);
			tltPoint.y = TP_MIN(tltPoint.y, set->objectList.at(i)->pos().y);
			trbPoint.x = TP_MAX(trbPoint.x, set->objectList.at(i)->pos().x + set->objectList.at(i)->width());
			trbPoint.y = TP_MAX(trbPoint.x, set->objectList.at(i)->pos().y + set->objectList.at(i)->height());
			set->objectRect[set->objectList.at(i)] = set->objectList.at(i)->rect();
		}

		set->ltPoint = tltPoint;
		set->rbPoint = trbPoint;

		uint32_t firstObjectX = set->ltPoint.x;

		uint32_t realWidth = set->rbPoint.x - set->ltPoint.x;
		uint32_t realHeight = set->rbPoint.y - set->ltPoint.y;

		if (set->ltPoint.x < 0)
		{
			set->offsetX = TP_ABS(set->ltPoint.x);
		}

		if (set->ltPoint.y < 0)
		{
			set->offsetY = TP_ABS(set->ltPoint.y);
		}

		if (realWidth < rectWidth)
		{
			realWidth = rectWidth;
		}

		if (realHeight < rectHeight)
		{
			realHeight = rectHeight;
		}

		if (set->ltPoint.x > 0)
		{
			set->ltPoint.x = 0;
		}

		if (set->ltPoint.y > 0)
		{
			set->ltPoint.y = 0;
		}

		int32_t scrollAreaX = realWidth - rectWidth;
		int32_t scrollAreaY = realHeight - rectHeight;

		// 最后panel的宽度要叠加上第一个成员的X偏移量 WHY
		uint32_t acutalPanelWidth = realWidth + firstObjectX;
		set->panel->setRect(0, 0, acutalPanelWidth, (int32_t)realHeight);
		// std::cout << "acutalPanelWidth " << acutalPanelWidth << "  realHeight  " << realHeight << std::endl;

		if (enableOffset)
		{
			auto iter = set->objectList.begin();

			for (; iter != set->objectList.end(); iter++)
			{
				(*iter)->move(set->offsetX + (*iter)->pos().x, set->offsetY + (*iter)->pos().y);
			}
		}

		if (realWidth >= rectWidth)
		{
			set->scroll[TP_LTSCR_INDEX_H]->setRect(0, this->height() - 5, this->width() - 5, 5);
			set->scroll[TP_LTSCR_INDEX_H]->setMaxRange(scrollAreaX);
			set->scroll[TP_LTSCR_INDEX_H]->setLinePerPage(scrollAreaX / 10);
			set->scroll[TP_LTSCR_INDEX_H]->setPosition(0);
			set->scroll[TP_LTSCR_INDEX_H]->setVisible(set->HScrollBarVisible);
			realWidth -= 5;
		}
		else
		{
			set->scroll[TP_LTSCR_INDEX_H]->setVisible(false);
		}

		if (realHeight >= rectHeight)
		{
			set->scroll[TP_RBSCR_INDEX_V]->setRect(this->width() - 5, 0, 5, this->height() - 5);
			set->scroll[TP_RBSCR_INDEX_V]->setMaxRange(scrollAreaY);
			set->scroll[TP_RBSCR_INDEX_V]->setLinePerPage(scrollAreaY / 10);
			set->scroll[TP_RBSCR_INDEX_V]->setPosition(0);
			set->scroll[TP_RBSCR_INDEX_V]->setVisible(set->VScrollBarVisible);
			realHeight -= 5;
		}
		else
		{
			set->scroll[TP_RBSCR_INDEX_V]->setVisible(false);
		}
	}

	// update();

	return ret;
}

void tpScrollPanel::doFlip(tpScroll *scroll, int32_t position, int32_t page, int32_t pageIndex, double percent)
{
	std::cout << "tpScrollPanel::doFlip " << std::endl;

	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	int32_t x = set->panel->pos().x;
	int32_t y = set->panel->pos().y;

	if (!set)
		return;

	std::cout << "position: " << position << std::endl;

	if (scroll == set->scroll[TP_LTSCR_INDEX_H])
	{
		x = scroll->min() - position;
	}
	else if (scroll == set->scroll[TP_RBSCR_INDEX_V])
	{
		y = scroll->min() - position;
	}
	else
	{
	}

	set->panel->move(x, y);
	set->panel->broadSetTop();
	tpChildWidget::update();

	// set->scroll[TP_LTSCR_INDEX]
}

bool tpScrollPanel::onPaintEvent(tpObjectPaintEvent *event)
{
	tpChildWidget::onPaintEvent(event);
	// recal(true);
	return true;
}

bool tpScrollPanel::eventFilter(tpObject *watched, tpEvent *event)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return false;

	if (event->eventType() == tpEvent::EVENT_MOUSE_PRESS_TYPE)
	{
		tpMouseEvent *mouseKeyEvent = dynamic_cast<tpMouseEvent *>(event);
		if (!mouseKeyEvent)
			return false;

		onMousePressEvent(mouseKeyEvent);
	}
	else if (event->eventType() == tpEvent::EVENT_MOUSE_RELEASE_TYPE)
	{
		tpMouseEvent *mouseKeyEvent = dynamic_cast<tpMouseEvent *>(event);
		if (!mouseKeyEvent)
			return false;

		onMouseRleaseEvent(mouseKeyEvent);
	}
	else if (event->eventType() == tpEvent::EVENT_MOUSE_MOVE_TYPE)
	{
		tpMouseEvent *mouseMotionEvent = dynamic_cast<tpMouseEvent *>(event);
		if (!mouseMotionEvent)
			return false;

		onMouseMoveEvent(mouseMotionEvent);
	}
	else if (event->eventType() == tpEvent::EVENT_WHEEL_EVENT)
	{
		tpWheelEvent *wheelEvent = dynamic_cast<tpWheelEvent *>(event);
		if (!wheelEvent)
			return false;

		onWheelEvent(wheelEvent);
	}
	else
	{
	}

	if (set->centralWidget == watched)
	{
		if (event->eventType() == tpEvent::EVENT_OBJECT_RESIZE_TYPE)
		{
			recal();
		}
	}

	return false;
}

bool tpScrollPanel::onMousePressEvent(tpMouseEvent *event)
{
	if (event->button() != BUTTON_LEFT)
		return true;

	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return false;

	// 鼠标左键点击，记录点击坐标
	set->mouseLeftPress = event->state();
	set->originPressPoint_ = event->globalPos();
	set->updatePoint_ = set->originPressPoint_;

	return true;
}

bool tpScrollPanel::onMouseRleaseEvent(tpMouseEvent *event)
{
	if (event->button() != BUTTON_LEFT)
		return true;

	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return false;

	set->mouseLeftPress = event->state();

	// 鼠标释放，如果拖拽的偏移边界了，回归0点位或终止位
	int32_t curScrollValue = scrollMode() ? verticalScrollBarValue() : horizontalScrollBarValue();
	if (curScrollValue <= 0)
	{
		curScrollValue = 0;
		scrollMode() ? setVerticalScrollBarValue(curScrollValue) : setHorizontalScrollBarValue(curScrollValue);
	}
	else if (curScrollValue >= 100)
	{
		curScrollValue = 100;
		scrollMode() ? setVerticalScrollBarValue(curScrollValue) : setHorizontalScrollBarValue(curScrollValue);
	}
	else
	{
	}

	return true;
}

bool tpScrollPanel::onMouseMoveEvent(tpMouseEvent *event)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return false;

	if (set->mouseLeftPress)
	{
		ItpPoint curPoint = event->globalPos();

		int32_t offset = 0;
		if (scrollMode())
		{
			// 纵向
			offset = curPoint.y - set->updatePoint_.y;

			setVerticalPostion(verticalPostion() + offset);
		}
		else
		{
			// 横向
			offset = curPoint.x - set->updatePoint_.x;

			// std::cout << "horizontalPostion() " << horizontalPostion() << " offset" << std::endl;
			setHorizontalPostion(horizontalPostion() + offset);
		}

		set->updatePoint_ = curPoint;

		tpChildWidget::update();
	}

	return true;
}

bool tpScrollPanel::onWheelEvent(tpWheelEvent *event)
{
	int32_t delta = event->angleDelta();

	if (delta > 0)
	{
		// 捕获鼠标滚轮事件,

		int32_t curScrollValue = scrollMode() ? verticalScrollBarValue() : horizontalScrollBarValue();
		std::cout << "curScrollValue : " << curScrollValue << std::endl;

		curScrollValue -= WHEEL_SCROLL_INTERVAL;

		if (curScrollValue < 0)
			curScrollValue = 0;

		scrollMode() ? setVerticalScrollBarValue(curScrollValue) : setHorizontalScrollBarValue(curScrollValue);
	}
	else
	{
		int32_t curScrollValue = scrollMode() ? verticalScrollBarValue() : horizontalScrollBarValue();
		std::cout << "curScrollValue : " << curScrollValue << std::endl;

		curScrollValue += WHEEL_SCROLL_INTERVAL;

		if (curScrollValue > 100)
			curScrollValue = 100;

		scrollMode() ? setVerticalScrollBarValue(curScrollValue) : setHorizontalScrollBarValue(curScrollValue);
	}

	return true;
}

bool tpScrollPanel::onResizeEvent(tpObjectResizeEvent *event)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return true;

	if (set->centralWidget)
	{
		set->centralWidget->setRect(0, 0, width(), height());
	}
	recal();

	return true;
}

bool tpScrollPanel::onLeaveEvent(tpObjectLeaveEvent *event)
{
	tpScrollPanelData *set = static_cast<tpScrollPanelData *>(data_);
	if (!set)
		return true;

	// std::cout << "event->leave() " << event->leave() << std::endl;
	if (!event->leave())
	{
		set->mouseLeftPress = false;

		// 鼠标释放，如果拖拽的偏移边界了，回归0点位或终止位
		int32_t curScrollValue = scrollMode() ? verticalScrollBarValue() : horizontalScrollBarValue();
		if (curScrollValue <= 0)
		{
			curScrollValue = 0;
			scrollMode() ? setVerticalScrollBarValue(curScrollValue) : setHorizontalScrollBarValue(curScrollValue);
		}
		else if (curScrollValue >= 100)
		{
			curScrollValue = 100;
			scrollMode() ? setVerticalScrollBarValue(curScrollValue) : setHorizontalScrollBarValue(curScrollValue);
		}
		else
		{
		}
	}

	return true;
}
