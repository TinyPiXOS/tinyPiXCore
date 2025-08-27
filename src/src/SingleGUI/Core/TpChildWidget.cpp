#include "TpChildWidget.h"
#include "TpApp.h"
#include "TpMessage.h"
#include "TpDef.h"
#include "TpVector.h"
#include "TpVariant.h"
#include "TpEvent.h"
#include "TpRect.h"
#include "TpLayout.h"
#include "TpCanvas.h"
#include "TpPoint.h"
#include "tinyPiXUtils.h"
#include "Core/TpObjectFunction.hpp"
#include "TpGlobal.h"
#include "TpDefaultCss.h"
#include "TpScreen.h"
#include "TpVirtualKeyboard.h"
#include "TpImage.h"
#include "thorVG/thorvg.h"

#include <unordered_map>
#include <mutex>

struct TpChildWidgetData
{
    tpShared<TpCssData> enabledCssData;
    tpShared<TpCssData> pressCssData;
    tpShared<TpCssData> hoverCssData;
    tpShared<TpCssData> checkedCssData;
    tpShared<TpCssData> disabledCssData;

    tvg::Scene *tvgScene = nullptr;
    tvg::SwCanvas *swCanvas = nullptr;

    TpChildWidgetData()
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

    if (set->reserveImage.isNull())
        return;

    set->cacheImage = set->reserveImage.scaled(set->logicalRect.w, set->logicalRect.h, set->keepAspectRatio);
    if (!set->cacheImage.isNull())
    {
        // 是否进行背景模糊
        if (set->enableBlur)
        {
            set->cacheImage.gaussianBlur(set->blurRadius);
        }
    }
}

static void changeXY(TpChildWidget *thisPtr, ItpObjectSet *set, const int32_t &x, const int32_t &y)
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
        TpObjectMoveEvent event;
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

static void changeWidth(TpChildWidget *thisPtr, ItpObjectSet *set, const uint32_t &w)
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
        input.question = TpObjectResizeEvent::TP_NORMAL_CHANGE;
        TpObjectResizeEvent event;
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

static void changeHeight(TpChildWidget *thisPtr, ItpObjectSet *set, const uint32_t &h)
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
        input.question = TpObjectResizeEvent::TP_NORMAL_CHANGE;
        TpObjectResizeEvent event;
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

TpChildWidget::TpChildWidget(TpChildWidget *parent)
    : TpObject(parent)
{
    data_ = new TpChildWidgetData();

    TpApp::Inst()->sendRegister(this);

    setParent(parent);
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

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

TpChildWidget::~TpChildWidget()
{
    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);
    if (childData)
    {
        delete childData;
        childData = nullptr;
        data_ = nullptr;
    }
}

void *TpChildWidget::testCanvasPtr()
{
    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);
    if (childData->swCanvas == nullptr)
    {
        childData->swCanvas = tvg::SwCanvas::gen();
        childData->tvgScene = tvg::Scene::gen();
        childData->swCanvas->push(std::move(childData->tvgScene));
    }
    return childData->swCanvas;
}

void *TpChildWidget::testScenePtr()
{
    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);
    if (childData->tvgScene == nullptr)
    {
        childData->swCanvas = tvg::SwCanvas::gen();
        childData->tvgScene = tvg::Scene::gen();
        childData->swCanvas->push(std::move(childData->tvgScene));
    }
    return childData->tvgScene;
}

void TpChildWidget::setProperty(const TpString &_name, const TpVariant &_value)
{
    TpObject::setProperty(_name, _value);

    // 如果更新控件type，更新样式
    if (_name.compare("type") == 0)
    {
        TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);

        childData->enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
        childData->pressCssData = readCss(pluginType(), TpCssParser::Pressed);
        childData->hoverCssData = readCss(pluginType(), TpCssParser::Hover);
        childData->checkedCssData = readCss(pluginType(), TpCssParser::Checked);
        childData->disabledCssData = readCss(pluginType(), TpCssParser::Disabled);

        refreshBaseCss();

        update();
    }
}

void TpChildWidget::deleteLater()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());

    // 顶层窗口缓存清除
    if (set->top)
    {
        ItpObjectSet *topData = static_cast<ItpObjectSet *>(set->top->objectSets());
        topData->tmp.deleteObject(this);

        TpList<TpObject *> thisChildList = this->objectList();
        for (const auto &child : thisChildList)
        {
            child->uninstallEventFilter();
            topData->tmp.deleteObject(child);
        }
    }

    setParent(nullptr);

    // set->tmp.deleteObject(this);

    uninstallEventFilter();

    TpObject::deleteLater();
    // TpApp::Inst()->sendDelete(this);
}

void TpChildWidget::close()
{
    setVisible(false);
}

void TpChildWidget::show()
{
    setVisible(true);
}

void TpChildWidget::showMaximum()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    uint32_t sWidth = 0;
    uint32_t sHeight = 0;
    tinyPiX_wf_get_display_size(set->agent, &sWidth, &sHeight);

    setRect(0, 0, sWidth, sHeight);
    setVisible(true);

    update();
}

void TpChildWidget::setVisible(bool visible)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());

    if (!set)
        return;

    if (visible == set->visible)
        return;

    set->visible = visible;
    ItpObjectVisibleSet input;

    input.object = this;
    input.visible = visible;

    TpObjectVisibleEvent event;
    bool ret = event.construct(&input);

    if (ret)
    {
        this->onVisibleEvent(&event);
    }
}

bool TpChildWidget::visible()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->visible;
}

void TpChildWidget::setEnabled(const bool &enable)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->enable = enable;

    // 所有子组件均要同步设置禁用状态
    for (const auto &childObj : set->objectList)
    {
        TpChildWidget *childWidget = dynamic_cast<TpChildWidget *>(childObj);
        if (!childWidget)
            continue;
        childWidget->setEnabled(enable);
    }
    update();
}

bool TpChildWidget::enabled()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->enable;
}

void TpChildWidget::setText(const TpString &text)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
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

TpString TpChildWidget::text()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return "";

    return set->text;
}

int32_t TpChildWidget::offsetX()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->offsetX;
}

int32_t TpChildWidget::offsetY()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->offsetY;
}

void TpChildWidget::setRect(const TpRect &rect)
{
    setRect(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

void TpChildWidget::setRect(const TpRect *rect)
{
    if (!rect)
        return;

    setRect(rect->X0(), rect->Y0(), rect->width(), rect->height());
}

void TpChildWidget::setRect(const ItpRect &rect)
{
    setRect(rect.x, rect.y, rect.w, rect.h);
}

void TpChildWidget::setRect(const ItpRect *rect)
{
    if (!rect)
        return;

    setRect(rect->x, rect->y, rect->w, rect->h);
}

void TpChildWidget::setRect(const int32_t &x, const int32_t &y, const uint32_t &w, const uint32_t &h)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    changeXY(this, set, x, y);

    setSize(w, h);
}

ItpRect TpChildWidget::toScreen()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return ItpRect();

    return set->absoluteRect;
}

ItpRect TpChildWidget::rect()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return ItpRect();

    return set->logicalRect;
}

ItpSize TpChildWidget::screenSize()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    uint32_t sWidth = 0;
    uint32_t sHeight = 0;
    tinyPiX_wf_get_display_size(set->agent, &sWidth, &sHeight);

    return ItpSize(sWidth, sHeight);
}

void TpChildWidget::setSize(const uint32_t &width, const uint32_t &height)
{
    setWidth(width);
    setHeight(height);
}

void TpChildWidget::setSize(const ItpSize &size)
{
    setSize(size.w, size.h);
}

ItpSize TpChildWidget::size()
{
    return ItpSize(width(), height());
}

void TpChildWidget::setWidth(const uint32_t &width)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    changeWidth(this, set, width);
}

void TpChildWidget::setHeight(const uint32_t &height)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    changeHeight(this, set, height);
}

uint32_t TpChildWidget::width()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->logicalRect.w;
}

uint32_t TpChildWidget::height()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->logicalRect.h;
}

void TpChildWidget::setMinimumSize(const uint32_t &width, const uint32_t &height)
{
    setMinumumWidth(width);
    setMinumumHeight(height);
}

void TpChildWidget::setMinimumSize(const ItpSize &minimumSize)
{
    setMinimumSize(minimumSize.w, minimumSize.h);
}

ItpSize TpChildWidget::minimumSize()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return ItpSize();
    return ItpSize(set->minimumWidth, set->minimumHeight);
}

void TpChildWidget::setMinumumWidth(const uint32_t &width)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->minimumWidth = width;

    if (this->width() < set->minimumWidth)
        setWidth(set->minimumWidth);
}

uint32_t TpChildWidget::minumumWidth()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;
    return set->minimumWidth;
}

void TpChildWidget::setMinumumHeight(const uint32_t &height)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    if (height == 1350)
        int a = 0;

    set->minimumHeight = height;

    if (this->height() < set->minimumHeight)
        setHeight(set->minimumHeight);
}

uint32_t TpChildWidget::minumumHeight()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;
    return set->minimumHeight;
}

void TpChildWidget::setMaximumSize(const uint32_t &width, const uint32_t &height)
{
    setMaxumumWidth(width);
    setMaxumumHeight(height);
}

void TpChildWidget::setMaximumSize(const ItpSize &maximumSize)
{
    setMaximumSize(maximumSize.w, maximumSize.h);
}

ItpSize TpChildWidget::maximumSize()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return ItpSize();
    return ItpSize(set->maximumWidth, set->maximumHeight);
}

void TpChildWidget::setMaxumumWidth(const uint32_t &width)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->maximumWidth = width;

    if (this->width() < set->maximumWidth)
        setWidth(set->maximumWidth);
}

uint32_t TpChildWidget::maxumumWidth()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;
    return set->maximumWidth;
}

void TpChildWidget::setMaxumumHeight(const uint32_t &height)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;
    set->maximumHeight = height;

    if (this->height() > set->maximumHeight)
        setHeight(set->maximumHeight);
}

uint32_t TpChildWidget::maxumumHeight()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;
    return set->maximumHeight;
}

void TpChildWidget::setFixedSize(const uint32_t &width, const uint32_t &height)
{
    setFixedWidth(width);
    setFixedHeight(height);
}

void TpChildWidget::setFixedWidth(const uint32_t &width)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->minimumWidth = width;
    set->maximumWidth = width;

    setMinumumWidth(width);
    setMaxumumWidth(width);
    setWidth(width);
}

void TpChildWidget::setFixedHeight(const uint32_t &height)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->minimumHeight = height;
    set->maximumHeight = height;

    setMinumumHeight(height);
    setMaxumumHeight(height);
    setHeight(height);
}

bool TpChildWidget::isFixedSize()
{
    return (isFixedWidth() && isFixedHeight());
}

bool TpChildWidget::isFixedWidth()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;
    return (set->minimumWidth == set->maximumWidth);
}

bool TpChildWidget::isFixedHeight()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;
    return (set->minimumHeight == set->maximumHeight);
}

void TpChildWidget::move(int32_t x, int32_t y)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    changeXY(this, set, x, y);
    // update();
}

const ItpPoint TpChildWidget::pos()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return ItpPoint();

    ItpPoint point = {-1, -1};

    point.x = set->logicalRect.x;
    point.y = set->logicalRect.y;

    return point;
}

void TpChildWidget::setAlpha(const uint8_t &alpha)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->alpha = alpha;
}

uint8_t TpChildWidget::alpha()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->alpha;
}

bool TpChildWidget::setLayout(TpLayout *layout)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
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

TpLayout *TpChildWidget::layout()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return nullptr;

    return set->layout;
}

void TpChildWidget::update(TpRect &rect, bool onlyBlit)
{
    update(rect.X0(), rect.Y0(), rect.width(), rect.height(), onlyBlit);
}

void TpChildWidget::update(ItpRect &rect, bool onlyBlit)
{
    update(rect.x, rect.y, rect.w, rect.h, onlyBlit);
}

void TpChildWidget::update(ItpRect *rect, bool onlyBlit)
{
    if (!rect)
        return;

    update(rect->x, rect->y, rect->w, rect->h, onlyBlit);
}

void TpChildWidget::update(int32_t x, int32_t y, int32_t w, int32_t h, bool onlyBlit)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    TpChildWidget *childWidgetPtr = static_cast<TpChildWidget *>(set->top);
    if (!childWidgetPtr)
        return;

    ItpPoint point = selfToScreenPoint(this, x, y);
    bool ret = true;

    x = point.x;
    y = point.y;

    TpRect blitRect(x, y, w, h);

    // if (clip)
    {
        ItpRect screenRect = childWidgetPtr->toScreen();
        TpRect selfRect(screenRect);
        ret = blitRect.intersect(selfRect);
    }

    if (ret)
    {
        // TpApp::Inst()->postUpdateEvent(this, x, y, w, h, onlyBlit);
        TpApp::Inst()->postUpdateEvent(childWidgetPtr, 0, 0, childWidgetPtr->width(), childWidgetPtr->height(), onlyBlit);

        // if (set->top != this)
        // {
        //     childWidgetPtr->update(blitRect, onlyBlit);
        // }
    }
}

void TpChildWidget::update(bool onlyBlit)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    update(set->logicalRect.x, set->logicalRect.y, set->logicalRect.w, set->logicalRect.h, onlyBlit);
}

void TpChildWidget::setCheckable(const bool &_checkable)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->checkable = _checkable;
}

bool TpChildWidget::checkable()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->checkable;
}

void TpChildWidget::setChecked(const bool &_isChecked)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->isChecked = _isChecked;
    update();
}

bool TpChildWidget::checked()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->isChecked;
}

void TpChildWidget::setRoundCorners(const uint32_t &round)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
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

uint32_t TpChildWidget::roundCorners()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->round;
}

TpImage TpChildWidget::backGroundCacheImage()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return TpImage();

    return set->cacheImage;
}

void TpChildWidget::setBackGroundImage(TpImage image, bool keepAspectRatio)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->reserveImage = image;
    set->enableImage = true;
    set->keepAspectRatio = keepAspectRatio;

    if (set->logicalRect.w != 0 && set->logicalRect.h != 0)
    {
        refreshCacheImage(set);
    }
    update();
}

TpImage TpChildWidget::backGroundImage()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return TpImage();

    return set->reserveImage;
}

bool TpChildWidget::enableBackGroundImage()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->enableImage;
}

void TpChildWidget::setEnableBackGroundImage(bool enable)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->enableImage = enable;
}

void TpChildWidget::setBackGroundColor(TpColors &color, bool enable)
{
    setBackGroundColor(color.rgba(), enable);
}

void TpChildWidget::setBackGroundColor(int32_t color, bool enable)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->backColor = color;
    set->enableColor = enable;

    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);

    // CSS解析完，初始化默认状态下CSS数据对象
    enabledCss()->setBackgroundColor(color);
    pressedCss()->setBackgroundColor(color);
    hoveredCss()->setBackgroundColor(color);
    checkedCss()->setBackgroundColor(color);
    disableCss()->setBackgroundColor(color);
}

uint32_t TpChildWidget::backGroundColor()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->backColor;
}

bool TpChildWidget::enableBackGroundColor()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->enableColor;
}

void TpChildWidget::setEnableBackGroundColor(bool enable)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->enableColor = enable;
}

void TpChildWidget::setBorderColor(TpColors &color, bool enable)
{
    setBorderColor(color.rgba(), enable);
}

void TpChildWidget::setBorderColor(int32_t color, bool enable)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->borderColor = color;
    set->enableBorderColor = enable;
}

uint32_t TpChildWidget::borderColor()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return _RGB(0, 0, 0);

    return set->borderColor;
}

bool TpChildWidget::enableBorderColor()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->enableBorderColor;
}

void TpChildWidget::setEnabledBorderColor(bool enable)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->enableBorderColor = enable;
}

void TpChildWidget::setBlurRadius(const uint32_t &blurRadius)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->blurRadius = blurRadius;
}

uint32_t TpChildWidget::blurRadius()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return 5;

    return set->blurRadius;
}

void TpChildWidget::setEnableBlur(const bool &enable)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return;

    set->enableBlur = enable;
}

bool TpChildWidget::enableBlur()
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->enableBlur;
}

void SetTopFunc(TpObject *topObj, ItpObjectSet *findSetData)
{
    for (const auto &setObj : findSetData->objectList)
    {
        ItpObjectSet *curSet = (ItpObjectSet *)setObj->objectSets();
        curSet->top = topObj;

        if (curSet->objectList.size() > 0)
            SetTopFunc(topObj, curSet);
    }
};

void TpChildWidget::setParent(TpObject *parent)
{
    TpObject::setParent(parent);

    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

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

bool TpChildWidget::onMousePressEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    set->isPress = true;
    set->pressPoint = event->globalPos();

    update();

    return true;
}

bool TpChildWidget::onMouseRleaseEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    set->isPress = false;

    ItpPoint mouseGlobalPos = event->globalPos();
    bool isUpdate = false;

    if ((std::abs(mouseGlobalPos.x - set->pressPoint.x) <= 5) && (std::abs(mouseGlobalPos.y - set->pressPoint.y) <= 5))
    {
        if (checkable())
        {
            std::cout << "当前选中状态： " << checked() << std::endl;
            setChecked(!checked());
            isUpdate = true;
        }
    }

    if (!isUpdate)
        update();

    return true;
}

bool TpChildWidget::onResizeEvent(TpObjectResizeEvent *event)
{
    TpObject *object = event->object();

    if (!object)
        return false;

    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
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

bool TpChildWidget::onLeaveEvent(TpObjectLeaveEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_OBJECT_LEAVE_TYPE)
    {
        ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());

        set->isHover = event->leave();
        set->isPress = false;
        update();
    }

    return true;
}

bool TpChildWidget::onPaintEvent(TpObjectPaintEvent *event)
{
    bool ret = event->isCanDraw();
    uint8_t alpha = 0xff;

    if (!ret)
        return false;

    TpCanvas *canvas = event->canvas();
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return false;

    ItpRect rect = event->rect();

    if (!set->visible)
        return ret;

    // 判断组件当前状态，决定取出哪个CSS样式
    tpShared<TpCssData> curCssData = currentStatusCss();

    // uint32_t minRad = (width() > height() ? height() : width()) * curCssData->roundCorners();
    uint32_t minRad = curCssData->roundCorners();

    if (set->enableColor)
    {
        if (objectType() == TP_FLOAT_OBJECT)
        {
            if ((curCssData->backgroundColor() & 0xff) != 0xff)
            {
                // canvas->erase();
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

    if (set->enableImage && !set->cacheImage.isNull())
    {
        // canvas->paintImage(width() / 2.0, 0, set->cacheImage, minRad);
        canvas->paintImage(0, 0, set->cacheImage, minRad);
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

void TpChildWidget::onThemeChangeEvent(TpThemeChangeEvent *event)
{
    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);
    if (!childData)
        return;

    // childData->enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
    // childData->pressCssData = readCss(pluginType(), TpCssParser::Pressed);
    // childData->hoverCssData = readCss(pluginType(), TpCssParser::Hover);
    // childData->checkedCssData = readCss(pluginType(), TpCssParser::Checked);
    // childData->disabledCssData = readCss(pluginType(), TpCssParser::Disabled);

    // setRoundCorners(childData->enabledCssData->roundCorners());
}

ItpObjectType TpChildWidget::objectType()
{
    return TP_CHILD_OBJECT;
}

TpChildWidget *TpChildWidget::find(TpPoint &point)
{
    return find(point.get().x, point.get().x);
}

TpChildWidget *TpChildWidget::find(ItpPoint *point)
{
    if (!point)
        return nullptr;

    return find(point->x, point->y);
}

TpChildWidget *TpChildWidget::find(int32_t x, int32_t y)
{
    ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());
    if (!set)
        return nullptr;

    TpChildWidget *object = this;

    TpRect absRect(set->absoluteRect);

    bool ret = absRect.in(x, y);

    if (ret)
        object = this;

    TpChildWidget *result = findObject(set, x, y);

    if (result)
        object = result;

    return object;
}

void TpChildWidget::setStyleSheet(const TpString &_styleSheetStr)
{
    // 解析CSS字符串
    TpApp::Inst()->cssParser()->parseCss(_styleSheetStr);

    if (objectType() != TP_TOP_OBJECT)
        refreshBaseCss();
}

TpString TpChildWidget::styleSheet()
{
    return TpApp::Inst()->cssParser()->cssStr();
}

tpShared<TpCssData> TpChildWidget::readCss(const TpString &_className, const TpCssParser::MouseStatus &_status)
{
    TpString uiType = property("type").toString();

    return TpApp::Inst()->cssParser()->readCss(_className, uiType, _status);
}

tpShared<TpCssData> TpChildWidget::currentStatusCss()
{
    tpShared<TpCssData> curCssData = disableCss();
    if (enabled())
    {
        curCssData = enabledCss();

        ItpObjectSet *set = static_cast<ItpObjectSet *>(TpObject::objectSets());

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

tpShared<TpCssData> TpChildWidget::enabledCss()
{
    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);
    if (childData == nullptr)
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    if (childData->enabledCssData == nullptr)
    {
        childData->enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
    }

    return childData->enabledCssData;
}

tpShared<TpCssData> TpChildWidget::disableCss()
{
    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);
    if (childData == nullptr)
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    if (childData->disabledCssData == nullptr)
    {
        childData->disabledCssData = readCss(pluginType(), TpCssParser::Disabled);
    }

    return childData->disabledCssData;
}

tpShared<TpCssData> TpChildWidget::hoveredCss()
{
    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);
    if (childData == nullptr)
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    if (childData->hoverCssData == nullptr)
    {
        childData->hoverCssData = readCss(pluginType(), TpCssParser::Hover);
    }

    return childData->hoverCssData;
}

tpShared<TpCssData> TpChildWidget::pressedCss()
{
    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);
    if (childData == nullptr)
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    if (childData->pressCssData == nullptr)
    {
        childData->pressCssData = readCss(pluginType(), TpCssParser::Pressed);
    }

    return childData->pressCssData;
}

tpShared<TpCssData> TpChildWidget::checkedCss()
{
    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);
    if (childData == nullptr)
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    if (childData->checkedCssData == nullptr)
    {
        childData->checkedCssData = readCss(pluginType(), TpCssParser::Checked);
    }

    return childData->checkedCssData;
}

void TpChildWidget::refreshBaseCss()
{
    // 每次刷新CSS要从配置文件重新读取，避免产生继承关系时，子类未刷新正确自己的CSS数据
    TpChildWidgetData *childData = static_cast<TpChildWidgetData *>(data_);
    childData->enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
    childData->disabledCssData = readCss(pluginType(), TpCssParser::Disabled);
    childData->hoverCssData = readCss(pluginType(), TpCssParser::Hover);
    childData->pressCssData = readCss(pluginType(), TpCssParser::Pressed);
    childData->checkedCssData = readCss(pluginType(), TpCssParser::Checked);

    tpShared<TpCssData> normalCss = currentStatusCss();
    setMinimumSize(normalCss->minimumWidth(), normalCss->minimumHeight());
    setMaximumSize(normalCss->maximumWidth(), normalCss->maximumHeight());
    setSize(normalCss->width(), normalCss->height());
    setRoundCorners(normalCss->roundCorners());
}

bool TpChildWidget::objectActive()
{
    return false;
}
