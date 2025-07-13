#include "tpChildWidget.h"
#include "tpApp.h"
#include "tpMessage.h"
#include "tpDef.h"
#include "tpVector.h"
#include "tpVariant.h"
#include "tpEvent.h"
#include "tpRect.h"
#include "tpLayout.h"
#include "tpSurface.h"
#include "tpCanvas.h"
#include "tpPoint.h"
#include "tinyPiXUtils.h"
#include "core/tpObjectFunction.hpp"
#include "tpGlobal.h"
#include "tpDefaultCss.h"
#include "tpScreen.h"
#include "tpVirtualKeyboard.h"

#include <unordered_map>
#include <mutex>

struct tpChildWidgetData
{
	tpShared<tpCssData> enabledCssData;
	tpShared<tpCssData> pressCssData;
	tpShared<tpCssData> hoverCssData;
	tpShared<tpCssData> checkedCssData;
	tpShared<tpCssData> disabledCssData;

	tpChildWidgetData()
	{
		enabledCssData = nullptr;
		pressCssData = nullptr;
		hoverCssData = nullptr;
		checkedCssData = nullptr;
		disabledCssData = nullptr;
	}
};

// 刷新缓存背景图
static void refreshCacheImage(ItpObjectSet *set)
{
	if (!set)
		return;

	if (!set->reserveImage)
	{
		set->cacheImage = nullptr;
		return;
	}

	set->cacheImage = set->reserveImage->scaled(set->logicalRect.w, set->logicalRect.h);
	if (set->cacheImage)
	{
		// 是否进行背景模糊
		if (set->enableBlur)
		{
			set->cacheImage->glassBlur(set->blurRadius);
		}
	}
}

static void changeXY(tpChildWidget *thisPtr, ItpObjectSet *set, const int32_t &x, const int32_t &y)
{
	if (!set)
		return;

	int32_t ox = set->logicalRect.x;
	int32_t oy = set->logicalRect.y;

	if (ox != x || oy != y)
	{
		set->logicalRect.x = x;
		set->logicalRect.y = y;

		ItpPoint point = selfToScreenPoint(thisPtr, x, y);

		set->absoluteRect.x = point.x;
		set->absoluteRect.y = point.y;

		ItpObjectMoveSet input;
		input.object = thisPtr;
		input.nx = x;
		input.ny = y;
		tpObjectMoveEvent event;
		bool ret = event.construct(&input);

		if (ret)
		{
			thisPtr->onMoveEvent(&event);
		}
	}

	if (set->parent)
	{
		thisPtr->broadSetTop();
	}
}

static void changeWidth(tpChildWidget *thisPtr, ItpObjectSet *set, const uint32_t &w)
{
	if (!set)
		return;

	uint32_t ow = set->logicalRect.w;

	uint32_t setW = w;

	if (setW > set->maximumWidth)
		setW = set->maximumWidth;
	else if (setW < set->minimumWidth)
		setW = set->minimumWidth;
	else
	{
	}

	if (ow != setW)
	{
		set->logicalRect.w = setW;
		set->absoluteRect.w = setW;

		ItpObjectResizeSet input;
		input.object = thisPtr;
		input.nw = setW;
		input.nh = set->logicalRect.h;
		input.question = tpObjectResizeEvent::TP_NORMAL_CHANGE;
		tpObjectResizeEvent event;
		bool ret = event.construct(&input);

		if (ret)
		{
			refreshCacheImage(set);

			IssueObjEvent(thisPtr, event, onResizeEvent, true);
		}
	}

	if (set->parent)
	{
		thisPtr->broadSetTop();
	}
}

static void changeHeight(tpChildWidget *thisPtr, ItpObjectSet *set, const uint32_t &h)
{
	if (!set)
		return;

	uint32_t oh = set->logicalRect.h;

	uint32_t setH = h;

	if (setH > set->maximumHeight)
		setH = set->maximumHeight;
	else if (setH < set->minimumHeight)
		setH = set->minimumHeight;
	else
	{
	}

	if (oh != setH)
	{
		set->logicalRect.h = setH;
		set->absoluteRect.h = setH;

		ItpObjectResizeSet input;
		input.object = thisPtr;
		input.nw = set->logicalRect.w;
		input.nh = setH;
		input.question = tpObjectResizeEvent::TP_NORMAL_CHANGE;
		tpObjectResizeEvent event;
		bool ret = event.construct(&input);

		// std::cout << "Change Height " << setH << std::endl;

		if (ret)
		{
			refreshCacheImage(set);

			IssueObjEvent(thisPtr, event, onResizeEvent, true);
		}
	}

	if (set->parent)
	{
		thisPtr->broadSetTop();
	}
}

tpChildWidget::tpChildWidget(tpChildWidget *parent)
	: tpObject(parent)
{
	data_ = new tpChildWidgetData();

	tpApp::Inst()->sendRegister(this);

	setParent(parent);
	ItpObjectSet *set = (ItpObjectSet *)tpObject::objectSets();

	if (set)
	{
		set->top = this->topObject();

		// if (set->top && parent)
		// if (parent)
		// {
		// 	set->parent = parent;
		// 	set->parent->add(this);
		// }

		// 遍历this的所有子节点，所有子节点查询一下top
		// if (set->top)
		// {
		// 	SetTopFunc(set->top, set);
		// }
	}

	setVisible(true);
}

tpChildWidget::~tpChildWidget()
{
	tpChildWidgetData *childData = static_cast<tpChildWidgetData *>(data_);
	if (childData)
	{
		delete childData;
		childData = nullptr;
		data_ = nullptr;
	}
}

void tpChildWidget::setProperty(const tpString &_name, const tpVariant &_value)
{
	tpObject::setProperty(_name, _value);

	// 如果更新控件type，更新样式
	if (_name.compare("type") == 0)
	{
		tpChildWidgetData *childData = static_cast<tpChildWidgetData *>(data_);

		childData->enabledCssData = readCss(pluginType(), tpCssParser::Enabled);
		childData->pressCssData = readCss(pluginType(), tpCssParser::Pressed);
		childData->hoverCssData = readCss(pluginType(), tpCssParser::Hover);
		childData->checkedCssData = readCss(pluginType(), tpCssParser::Checked);
		childData->disabledCssData = readCss(pluginType(), tpCssParser::Disabled);

		refreshBaseCss();

		update();
	}
}

void tpChildWidget::deleteLater()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());

	// 顶层窗口缓存清除
	if (set->top)
	{
		ItpObjectSet *topData = static_cast<ItpObjectSet *>(set->top->objectSets());
		topData->tmp.deleteObject(this);

		tpList<tpObject *> thisChildList = this->objectList();
		for (const auto &child : thisChildList)
		{
			child->uninstallEventFilter();
			topData->tmp.deleteObject(child);
		}
	}

	setParent(nullptr);

	// set->tmp.deleteObject(this);

	uninstallEventFilter();

	tpObject::deleteLater();
	// tpApp::Inst()->sendDelete(this);
}

void tpChildWidget::close()
{
	setVisible(false);
}

void tpChildWidget::show()
{
	setVisible(true);
}

void tpChildWidget::showMaximum()
{
	ItpObjectSet *set = (ItpObjectSet *)tpObject::objectSets();
	uint32_t sWidth = 0;
	uint32_t sHeight = 0;
	tinyPiX_wf_get_rotate_metrics(set->agent, &sWidth, &sHeight);

	setRect(0, 0, sWidth, sHeight);
	setVisible(true);

	update();
}

void tpChildWidget::setVisible(bool visible)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());

	if (!set)
		return;

	if (visible == set->visible)
		return;

	set->visible = visible;
	ItpObjectVisibleSet input;

	input.object = this;
	input.visible = visible;

	tpObjectVisibleEvent event;
	bool ret = event.construct(&input);

	if (ret)
	{
		this->onVisibleEvent(&event);
	}
}

bool tpChildWidget::visible()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	return set->visible;
}

void tpChildWidget::setEnabled(const bool &enable)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->enable = enable;

	// 所有子组件均要同步设置禁用状态
	for (const auto &childObj : set->objectList)
	{
		tpChildWidget *childWidget = dynamic_cast<tpChildWidget *>(childObj);
		if (!childWidget)
			continue;
		childWidget->setEnabled(enable);
	}
	update();
}

bool tpChildWidget::enabled()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	return set->enable;
}

void tpChildWidget::setText(const tpString &text)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	if (text.length() < OBJECT_MAX_TEXT_LENGTH - 1)
	{
		memcpy(set->text, text.c_str(), text.length());
		set->text[text.length()] = '\0'; // 确保字符串以空字符结尾
	}
	else
	{
		memcpy(set->text, text.c_str(), OBJECT_MAX_TEXT_LENGTH - 1);
		set->text[OBJECT_MAX_TEXT_LENGTH - 1] = '\0'; // 防止缓冲区溢出
	}
}

tpString tpChildWidget::text()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return "";

	return set->text;
}

int32_t tpChildWidget::offsetX()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;

	return set->offsetX;
}

int32_t tpChildWidget::offsetY()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;

	return set->offsetY;
}

void tpChildWidget::setRect(const tpRect &rect)
{
	setRect(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

void tpChildWidget::setRect(const tpRect *rect)
{
	if (!rect)
		return;

	setRect(rect->X0(), rect->Y0(), rect->width(), rect->height());
}

void tpChildWidget::setRect(const ItpRect &rect)
{
	setRect(rect.x, rect.y, rect.w, rect.h);
}

void tpChildWidget::setRect(const ItpRect *rect)
{
	if (!rect)
		return;

	setRect(rect->x, rect->y, rect->w, rect->h);
}

void tpChildWidget::setRect(const int32_t &x, const int32_t &y, const uint32_t &w, const uint32_t &h)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	changeXY(this, set, x, y);

	setSize(w, h);
}

ItpRect tpChildWidget::toScreen()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return ItpRect();

	return set->absoluteRect;
}

ItpRect tpChildWidget::rect()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return ItpRect();

	return set->logicalRect;
}

ItpSize tpChildWidget::screenSize()
{
	ItpObjectSet *set = (ItpObjectSet *)tpObject::objectSets();
	uint32_t sWidth = 0;
	uint32_t sHeight = 0;
	tinyPiX_wf_get_rotate_metrics(set->agent, &sWidth, &sHeight);

	return ItpSize(sWidth, sHeight);
}

void tpChildWidget::setSize(const uint32_t &width, const uint32_t &height)
{
	setWidth(width);
	setHeight(height);
}

void tpChildWidget::setSize(const ItpSize &size)
{
	setSize(size.w, size.h);
}

ItpSize tpChildWidget::size()
{
	return ItpSize(width(), height());
}

void tpChildWidget::setWidth(const uint32_t &width)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	changeWidth(this, set, width);
}

void tpChildWidget::setHeight(const uint32_t &height)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	changeHeight(this, set, height);
}

uint32_t tpChildWidget::width()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;

	return set->logicalRect.w;
}

uint32_t tpChildWidget::height()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;

	return set->logicalRect.h;
}

void tpChildWidget::setMinimumSize(const uint32_t &width, const uint32_t &height)
{
	setMinumumWidth(width);
	setMinumumHeight(height);
}

void tpChildWidget::setMinimumSize(const ItpSize &minimumSize)
{
	setMinimumSize(minimumSize.w, minimumSize.h);
}

ItpSize tpChildWidget::minimumSize()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return ItpSize();
	return ItpSize(set->minimumWidth, set->minimumHeight);
}

void tpChildWidget::setMinumumWidth(const uint32_t &width)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->minimumWidth = width;

	if (this->width() < set->minimumWidth)
		setWidth(set->minimumWidth);
}

uint32_t tpChildWidget::minumumWidth()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;
	return set->minimumWidth;
}

void tpChildWidget::setMinumumHeight(const uint32_t &height)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	if (height == 1350)
		int a = 0;

	set->minimumHeight = height;

	if (this->height() < set->minimumHeight)
		setHeight(set->minimumHeight);
}

uint32_t tpChildWidget::minumumHeight()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;
	return set->minimumHeight;
}

void tpChildWidget::setMaximumSize(const uint32_t &width, const uint32_t &height)
{
	setMaxumumWidth(width);
	setMaxumumHeight(height);
}

void tpChildWidget::setMaximumSize(const ItpSize &maximumSize)
{
	setMaximumSize(maximumSize.w, maximumSize.h);
}

ItpSize tpChildWidget::maximumSize()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return ItpSize();
	return ItpSize(set->maximumWidth, set->maximumHeight);
}

void tpChildWidget::setMaxumumWidth(const uint32_t &width)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->maximumWidth = width;

	if (this->width() < set->maximumWidth)
		setWidth(set->maximumWidth);
}

uint32_t tpChildWidget::maxumumWidth()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;
	return set->maximumWidth;
}

void tpChildWidget::setMaxumumHeight(const uint32_t &height)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;
	set->maximumHeight = height;

	if (this->height() > set->maximumHeight)
		setHeight(set->maximumHeight);
}

uint32_t tpChildWidget::maxumumHeight()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;
	return set->maximumHeight;
}

void tpChildWidget::setFixedSize(const uint32_t &width, const uint32_t &height)
{
	setFixedWidth(width);
	setFixedHeight(height);
}

void tpChildWidget::setFixedWidth(const uint32_t &width)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->minimumWidth = width;
	set->maximumWidth = width;

	setMinumumWidth(width);
	setMaxumumWidth(width);
	setWidth(width);
}

void tpChildWidget::setFixedHeight(const uint32_t &height)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->minimumHeight = height;
	set->maximumHeight = height;

	setMinumumHeight(height);
	setMaxumumHeight(height);
	setHeight(height);
}

bool tpChildWidget::isFixedSize()
{
	return (isFixedWidth() && isFixedHeight());
}

bool tpChildWidget::isFixedWidth()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;
	return (set->minimumWidth == set->maximumWidth);
}

bool tpChildWidget::isFixedHeight()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;
	return (set->minimumHeight == set->maximumHeight);
}

void tpChildWidget::move(int32_t x, int32_t y)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	changeXY(this, set, x, y);
	// update();
}

const ItpPoint tpChildWidget::pos()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return ItpPoint();

	ItpPoint point = {-1, -1};

	point.x = set->logicalRect.x;
	point.y = set->logicalRect.y;

	return point;
}

void tpChildWidget::setAlpha(const uint8_t &alpha)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->alpha = alpha;
}

uint8_t tpChildWidget::alpha()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;

	return set->alpha;
}

bool tpChildWidget::setLayout(tpLayout *layout)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	if (!layout)
		return false;

	if (layout == set->layout)
		return false;

	if (set->layout)
		return false;

	if (set->layoutMutex.try_lock())
	{
		// set->layoutMutex.lock();

		// if (set->layout)
		// {
		// 	delObject(set, set->layout);
		// 	set->layout = nullptr;
		// }

		set->layout = layout;
		layout->setParent(this);
		set->layout->update();

		// addObject(set, set->layout, this);

		set->layoutMutex.unlock();
	}

	return true;
}

tpLayout *tpChildWidget::layout()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return nullptr;

	return set->layout;
}

void tpChildWidget::update(tpRect &rect, bool clip, bool onlyBlit, bool sync)
{
	update(rect.X0(), rect.Y0(), rect.width(), rect.height(), clip, onlyBlit, sync);
}

void tpChildWidget::update(ItpRect &rect, bool clip, bool onlyBlit, bool sync)
{
	update(rect.x, rect.y, rect.w, rect.h, clip, onlyBlit, sync);
}

void tpChildWidget::update(ItpRect *rect, bool clip, bool onlyBlit, bool sync)
{
	if (!rect)
		return;

	update(rect->x, rect->y, rect->w, rect->h, clip, onlyBlit, sync);
}

void tpChildWidget::update(int32_t x, int32_t y, int32_t w, int32_t h, bool clip, bool onlyBlit, bool sync)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	tpChildWidget *childWidgetPtr = static_cast<tpChildWidget *>(set->top);
	if (!childWidgetPtr)
		return;

	ItpPoint point = selfToScreenPoint(this, x, y);
	bool ret = true;

	x = point.x;
	y = point.y;

	tpRect blitRect(x, y, w, h);

	if (clip)
	{
		ItpRect screenRect = childWidgetPtr->toScreen();
		tpRect selfRect(screenRect);
		ret = blitRect.intersect(selfRect);
	}

	if (ret)
	{
		if (set->top != this)
		{
			childWidgetPtr->update(blitRect, clip, onlyBlit, sync);
		}
	}
}

void tpChildWidget::update(bool clip, bool onlyBlit, bool sync)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	update(set->logicalRect.x, set->logicalRect.y, set->logicalRect.w, set->logicalRect.h, clip, onlyBlit, sync);
}

void tpChildWidget::setCheckable(const bool &_checkable)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->checkable = _checkable;
}

bool tpChildWidget::checkable()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	return set->checkable;
}

void tpChildWidget::setChecked(const bool &_isChecked)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->isChecked = _isChecked;
	update();
}

bool tpChildWidget::checked()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	return set->isChecked;
}

void tpChildWidget::setRoundCorners(const uint32_t &round)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->round = round;

	// CSS解析完，初始化默认状态下CSS数据对象
	enabledCss()->setRoundCorners(round);
	pressedCss()->setRoundCorners(round);
	hoveredCss()->setRoundCorners(round);
	checkedCss()->setRoundCorners(round);
	disableCss()->setRoundCorners(round);
}

uint32_t tpChildWidget::roundCorners()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;

	return set->round;
}

tpShared<tpSurface> tpChildWidget::backGroundCacheImage()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return nullptr;

	return set->cacheImage;
}

void tpChildWidget::setBackGroundImage(tpShared<tpSurface> image, bool enable)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->reserveImage = image;
	set->enableImage = enable;

	if (set->logicalRect.w != 0 &&
		set->logicalRect.h != 0)
	{
		refreshCacheImage(set);
	}
}

tpShared<tpSurface> tpChildWidget::backGroundImage()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return nullptr;

	return set->reserveImage;
}

bool tpChildWidget::enableBackGroundImage()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	return set->enableImage;
}

void tpChildWidget::setEnableBackGroundImage(bool enable)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->enableImage = enable;
}

void tpChildWidget::setBackGroundColor(tpColors &color, bool enable)
{
	setBackGroundColor(color.rgba(), enable);
}

void tpChildWidget::setBackGroundColor(int32_t color, bool enable)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->backColor = color;
	set->enableColor = enable;

	tpChildWidgetData *childData = static_cast<tpChildWidgetData *>(data_);

	// CSS解析完，初始化默认状态下CSS数据对象
	enabledCss()->setBackgroundColor(color);
	pressedCss()->setBackgroundColor(color);
	hoveredCss()->setBackgroundColor(color);
	checkedCss()->setBackgroundColor(color);
	disableCss()->setBackgroundColor(color);
}

uint32_t tpChildWidget::backGroundColor()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;

	return set->backColor;
}

bool tpChildWidget::enableBackGroundColor()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	return set->enableColor;
}

void tpChildWidget::setEnableBackGroundColor(bool enable)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->enableColor = enable;
}

void tpChildWidget::setBorderColor(tpColors &color, bool enable)
{
	setBorderColor(color.rgba(), enable);
}

void tpChildWidget::setBorderColor(int32_t color, bool enable)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->borderColor = color;
	set->enableBorderColor = enable;
}

uint32_t tpChildWidget::borderColor()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return _RGB(0, 0, 0);

	return set->borderColor;
}

bool tpChildWidget::enableBorderColor()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	return set->enableBorderColor;
}

void tpChildWidget::setEnabledBorderColor(bool enable)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->enableBorderColor = enable;
}

void tpChildWidget::setBlurRadius(const uint32_t &blurRadius)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->blurRadius = blurRadius;
}

uint32_t tpChildWidget::blurRadius()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 5;

	return set->blurRadius;
}

void tpChildWidget::setEnableBlur(const bool &enable)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->enableBlur = enable;
}

bool tpChildWidget::enableBlur()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	return set->enableBlur;
}

void tpChildWidget::setColorKey(bool enable, int32_t colorKey)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->colorKey = colorKey;
	set->enableColorKey = enable;
}

void tpChildWidget::setColorKeyEnable(bool enable)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	set->enableColorKey = enable;
}

bool tpChildWidget::colorKeyEnable()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	return set->enableColorKey;
}

uint32_t tpChildWidget::colorKey()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return 0;

	return set->colorKey;
}

void tpChildWidget::setVarShape(void *shape)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return;

	if (set->varShape == shape)
		return;

	if (set->varSize)
		tinyPiX_bit_array_free(set->varShape);

	set->varSize = 0;
	set->varShape = nullptr;

	if (shape)
	{
		set->varShape = shape;
		set->varSize = tinyPiX_bit_array_get_size_by_bytes(shape);
	}
}

void *tpChildWidget::varShape()
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return nullptr;

	return set->varShape;
}

void SetTopFunc(tpObject *topObj, ItpObjectSet *findSetData)
{
	for (const auto &setObj : findSetData->objectList)
	{
		ItpObjectSet *curSet = (ItpObjectSet *)setObj->objectSets();
		curSet->top = topObj;

		if (curSet->objectList.size() > 0)
			SetTopFunc(topObj, curSet);
	}
};

void tpChildWidget::setParent(tpObject *parent)
{
	tpObject::setParent(parent);

	ItpObjectSet *set = (ItpObjectSet *)tpObject::objectSets();

	if (!set)
		return;

	set->top = this->topObject();

	if (parent == nullptr)
		set->top = nullptr;

	// 遍历this的所有子节点，所有子节点查询一下top
	if (set->top)
	{
		SetTopFunc(set->top, set);
	}
}

bool tpChildWidget::onMousePressEvent(tpMouseEvent *event)
{
	if (event->button() != BUTTON_LEFT)
		return true;

	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	set->isPress = true;
	update();

	return true;
}

bool tpChildWidget::onMouseRleaseEvent(tpMouseEvent *event)
{
	if (event->button() != BUTTON_LEFT)
		return true;

	ItpPoint mouseGlobalPos = event->globalPos();
	if (toScreen().contains(mouseGlobalPos))
	{
		if (checkable())
		{
			setChecked(!checked());
		}
	}
	update();

	return true;
}

bool tpChildWidget::onResizeEvent(tpObjectResizeEvent *event)
{
	tpObject *object = event->object();

	if (!object)
		return false;

	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	if (set->layoutMutex.try_lock())
	{
		// set->layoutMutex.lock();

		if (set->layout)
		{
			set->layout->update();
		}

		set->layoutMutex.unlock();
	}

	return true;
}

bool tpChildWidget::onLeaveEvent(tpObjectLeaveEvent *event)
{
	if (event->eventType() == tpEvent::EVENT_OBJECT_LEAVE_TYPE)
	{
		ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());

		set->isHover = event->leave();
		set->isPress = false;
		update();
	}

	return true;
}

bool tpChildWidget::onRotateEvent(tpObjectRotateEvent *event)
{
	tpObject *object = event->object();

	if (!object)
		return true;

	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return true;

	set->layoutMutex.lock();

	if (set->layout)
	{
		set->layout->update();
	}

	set->layoutMutex.unlock();

	return true;
}

bool tpChildWidget::onPaintEvent(tpObjectPaintEvent *event)
{
	bool ret = event->isCanDraw();
	uint8_t alpha = 0xff;

	if (!ret)
		return false;

	tpCanvas *canvas = event->canvas();
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return false;

	ItpRect rect = event->rect();

	if (!set->visible)
		return ret;

	// 判断组件当前状态，决定取出哪个CSS样式
	tpShared<tpCssData> curCssData = currentStatusCss();

	// uint32_t minRad = (width() > height() ? height() : width()) * curCssData->roundCorners();
	uint32_t minRad = curCssData->roundCorners();

	if (set->enableColor)
	{
		if (objectType() == TP_FLOAT_OBJECT)
		{
			if ((curCssData->backgroundColor() & 0xff) != 0xff)
			{
				canvas->erase();
			}
		}

		if (minRad == 0)
		{
			canvas->box(0, 0, rect.w, rect.h, curCssData->backgroundColor());
		}
		else
		{
			canvas->roundedBox(0, 0, rect.w, rect.h, minRad, curCssData->backgroundColor());
		}
	}

	if (set->enableBorderColor)
	{
		if (minRad == 0)
			canvas->rectangle(0, 0, rect.w - 1, rect.h - 1, curCssData->borderColor());
		else
			canvas->roundedRectangle(0, 0, rect.w - 1, rect.h - 1, minRad, curCssData->borderColor());
	}

	if (set->enableImage && set->cacheImage)
	{
		// uint8_t reservAlpha = set->cacheImage->alpha();
		// alpha = tpHelper::mapAlpha(set->cacheImage->alpha(), set->alpha);
		// set->cacheImage->setAlpha(alpha);
		// set->cacheImage->setColorKey(set->colorKey, set->enableColorKey);

		// if (set->enableBlur)

		if (minRad == 0)
		{
			canvas->paintSurface(0, 0, set->cacheImage);

			// canvas->paintSurface(set->cacheImage, nullptr, nullptr, false);
		}
		else
		{
			canvas->paintRoundSurface(0, 0, minRad, set->cacheImage);
		}

		// set->cacheImage->setAlpha(reservAlpha);
	}

	// 窗体更新，如果有布局更新布局
	if (set->layout)
	{
		if (set->layoutMutex.try_lock())
		{
			set->layout->update();

			set->layoutMutex.unlock();
		}
	}

	return ret;
}

void tpChildWidget::onThemeChangeEvent(tpThemeChangeEvent *event)
{
	tpChildWidgetData *childData = static_cast<tpChildWidgetData *>(data_);
	if (!childData)
		return;

	// childData->enabledCssData = readCss(pluginType(), tpCssParser::Enabled);
	// childData->pressCssData = readCss(pluginType(), tpCssParser::Pressed);
	// childData->hoverCssData = readCss(pluginType(), tpCssParser::Hover);
	// childData->checkedCssData = readCss(pluginType(), tpCssParser::Checked);
	// childData->disabledCssData = readCss(pluginType(), tpCssParser::Disabled);

	// setRoundCorners(childData->enabledCssData->roundCorners());
}

ItpObjectType tpChildWidget::objectType()
{
	return TP_CHILD_OBJECT;
}

bool tpChildWidget::appChange(int32_t id, int32_t pid, int32_t rotate, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require)
{
	return true;
}

tpChildWidget *tpChildWidget::find(tpPoint &point)
{
	return find(point.get().x, point.get().x);
}

tpChildWidget *tpChildWidget::find(ItpPoint *point)
{
	if (!point)
		return nullptr;

	return find(point->x, point->y);
}

tpChildWidget *tpChildWidget::find(int32_t x, int32_t y)
{
	ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());
	if (!set)
		return nullptr;

	tpChildWidget *object = this;

	tpRect absRect(set->absoluteRect);

	bool ret = absRect.in(x, y);

	if (ret)
		object = this;

	tpChildWidget *result = findObject(set, x, y);

	if (result)
		object = result;

	return object;
}

void tpChildWidget::setStyleSheet(const tpString &_styleSheetStr)
{
	// 解析CSS字符串
	tpApp::Inst()->cssParser()->parseCss(_styleSheetStr);

	if (objectType() != TP_TOP_OBJECT)
		refreshBaseCss();
}

tpString tpChildWidget::styleSheet()
{
	return tpApp::Inst()->cssParser()->cssStr();
}

tpShared<tpCssData> tpChildWidget::readCss(const tpString &_className, const tpCssParser::MouseStatus &_status)
{
	tpString uiType = property("type").toString();

	return tpApp::Inst()->cssParser()->readCss(_className, uiType, _status);
}

tpShared<tpCssData> tpChildWidget::currentStatusCss()
{
	tpShared<tpCssData> curCssData = disableCss();
	if (enabled())
	{
		curCssData = enabledCss();

		ItpObjectSet *set = static_cast<ItpObjectSet *>(tpObject::objectSets());

		if (set->checkable && set->isChecked)
		{
			curCssData = checkedCss();
		}
		else
		{
			if (set->isHover)
			{
				curCssData = hoveredCss();

				if (set->isPress)
					curCssData = pressedCss();
			}
		}
	}

	return curCssData;
}

tpShared<tpCssData> tpChildWidget::enabledCss()
{
	tpChildWidgetData *childData = static_cast<tpChildWidgetData *>(data_);
	if (childData == nullptr)
		return tpMakeShared<tpCssData>(tpHash<tpString, tpString>{});

	if (childData->enabledCssData == nullptr)
	{
		childData->enabledCssData = readCss(pluginType(), tpCssParser::Enabled);
	}

	return childData->enabledCssData;
}

tpShared<tpCssData> tpChildWidget::disableCss()
{
	tpChildWidgetData *childData = static_cast<tpChildWidgetData *>(data_);
	if (childData == nullptr)
		return tpMakeShared<tpCssData>(tpHash<tpString, tpString>{});

	if (childData->disabledCssData == nullptr)
	{
		childData->disabledCssData = readCss(pluginType(), tpCssParser::Disabled);
	}

	return childData->disabledCssData;
}

tpShared<tpCssData> tpChildWidget::hoveredCss()
{
	tpChildWidgetData *childData = static_cast<tpChildWidgetData *>(data_);
	if (childData == nullptr)
		return tpMakeShared<tpCssData>(tpHash<tpString, tpString>{});

	if (childData->hoverCssData == nullptr)
	{
		childData->hoverCssData = readCss(pluginType(), tpCssParser::Hover);
	}

	return childData->hoverCssData;
}

tpShared<tpCssData> tpChildWidget::pressedCss()
{
	tpChildWidgetData *childData = static_cast<tpChildWidgetData *>(data_);
	if (childData == nullptr)
		return tpMakeShared<tpCssData>(tpHash<tpString, tpString>{});

	if (childData->pressCssData == nullptr)
	{
		childData->pressCssData = readCss(pluginType(), tpCssParser::Pressed);
	}

	return childData->pressCssData;
}

tpShared<tpCssData> tpChildWidget::checkedCss()
{
	tpChildWidgetData *childData = static_cast<tpChildWidgetData *>(data_);
	if (childData == nullptr)
		return tpMakeShared<tpCssData>(tpHash<tpString, tpString>{});

	if (childData->checkedCssData == nullptr)
	{
		childData->checkedCssData = readCss(pluginType(), tpCssParser::Checked);
	}

	return childData->checkedCssData;
}

void tpChildWidget::refreshBaseCss()
{
	// 每次刷新CSS要从配置文件重新读取，避免产生继承关系时，子类未刷新正确自己的CSS数据
	tpChildWidgetData *childData = static_cast<tpChildWidgetData *>(data_);
	childData->enabledCssData = readCss(pluginType(), tpCssParser::Enabled);
	childData->disabledCssData = readCss(pluginType(), tpCssParser::Disabled);
	childData->hoverCssData = readCss(pluginType(), tpCssParser::Hover);
	childData->pressCssData = readCss(pluginType(), tpCssParser::Pressed);
	childData->checkedCssData = readCss(pluginType(), tpCssParser::Checked);

	tpShared<tpCssData> normalCss = currentStatusCss();
	setMinimumSize(normalCss->minimumWidth(), normalCss->minimumHeight());
	setMaximumSize(normalCss->maximumWidth(), normalCss->maximumHeight());
	setSize(normalCss->width(), normalCss->height());
	setRoundCorners(normalCss->roundCorners());
}

bool tpChildWidget::objectActive()
{
	return false;
}
