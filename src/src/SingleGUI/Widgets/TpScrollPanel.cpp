#include "TpScrollPanel.h"
#include "TpScroll.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "TpRect.h"
#include "TpMap.h"
#include "TpVector.h"
#include <climits>

#define TP_SCROLL_NUM 2
#define TP_LTSCR_INDEX_H 0
#define TP_RBSCR_INDEX_V 1

#ifndef WHEEL_SCROLL_INTERVAL
#define WHEEL_SCROLL_INTERVAL 10
#endif

struct TpScrollPanelData
{
    TpScroll *scroll[TP_SCROLL_NUM];
    TpChildWidget *panel;
    TpVector<TpChildWidget *> objectList;
    TpMap<TpChildWidget *, TpRect> objectRect;

    // 滚动窗口
    TpChildWidget *centralWidget = nullptr;

    // 水平和垂直滚动条是否显示
    bool HScrollBarVisible = true;
    bool VScrollBarVisible = true;

    // 滚轮滚动模式,true为垂直滚动
    bool isVertical = true;

    TpPoint ltPoint;
    TpPoint rbPoint;

    int32_t offsetX;
    int32_t offsetY;

    // 鼠标左键是否按下
    bool mouseLeftPress = false;
    // 鼠标点击坐标
    TpPoint originPressPoint_;
    // 鼠标拖拽更新坐标
    TpPoint updatePoint_;
};

TpScrollPanel::TpScrollPanel(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    TpScrollPanelData *set = new TpScrollPanelData();

    if (!set)
        return;

    set->panel = new TpChildWidget(this);
    set->panel->setEnableBackGroundColor(true);
    set->panel->setEnableBackGroundImage(false);
    set->panel->setEnabledBorderColor(false);
    set->panel->setVisible(true);

    set->panel->installEventFilter(this);
    // set->panel->setBackGroundColor(_RGB(0, 255, 0));
    for (int32_t i = 0; i < TP_SCROLL_NUM; i++)
    {
        set->scroll[i] = new TpScroll(this, i);
        set->scroll[i]->setVisible(false);
    }

    set->HScrollBarVisible = false;
    set->VScrollBarVisible = false;

    set->ltPoint.setX(INT_MAX);
    set->ltPoint.setY(INT_MAX);

    set->rbPoint.setX(INT_MIN);
    set->rbPoint.setY(INT_MIN);

    set->offsetX = 0;
    set->offsetY = 0;

    // connect(set->scroll[TP_LTSCR_INDEX_H], SIGNALS(TpScroll, onScroll, TpScroll *, int32_t, int32_t, int32_t, double), this, SLOTS(TpScrollPanel, doFlip, TpScroll *, int32_t, int32_t, int32_t, double));
    // connect(set->scroll[TP_RBSCR_INDEX_V], SIGNALS(TpScroll, onScroll, TpScroll *, int32_t, int32_t, int32_t, double), this, SLOTS(TpScrollPanel, doFlip, TpScroll *, int32_t, int32_t, int32_t, double));

    data_ = set;

    setVisible(true);
}

TpScrollPanel::~TpScrollPanel()
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);

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

uint32_t TpScrollPanel::horizontalScrollBarValue()
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return 0;

    TpScroll *scrollBar = set->scroll[TP_LTSCR_INDEX_H];
    uint32_t deltaPos = scrollBar->max() - scrollBar->min();
    if (deltaPos == 0)
        return 0;

    uint32_t finalPercent = 100 * (scrollBar->position() - scrollBar->min()) / deltaPos;

    return finalPercent;
}

void TpScrollPanel::setHorizontalScrollBarValue(const uint32_t &value)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return;

    TpScroll *scrollBar = set->scroll[TP_LTSCR_INDEX_H];

    uint32_t deltaPos = scrollBar->max() - scrollBar->min();
    uint32_t newPosion = value * deltaPos / 100.0;

    scrollBar->setPosition(newPosion);

    // std::cout << "newPosion : " << newPosion << std::endl;

    if (set->centralWidget)
    {
        int32_t x = set->centralWidget->pos().x();
        int32_t y = set->centralWidget->pos().y();

        set->centralWidget->move(-newPosion, y);
        set->centralWidget->broadSetTop();
    }
    else
    {
        int32_t x = set->panel->pos().x();
        int32_t y = set->panel->pos().y();

        set->panel->move(-newPosion, y);
        set->panel->broadSetTop();
    }

    TpChildWidget::update();
}

uint32_t TpScrollPanel::verticalScrollBarValue()
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return 0;

    TpScroll *scrollBar = set->scroll[TP_RBSCR_INDEX_V];
    uint32_t deltaPos = scrollBar->max() - scrollBar->min();

    if (deltaPos == 0)
        return 0;

    uint32_t finalPercent = 100 * (scrollBar->position() - scrollBar->min()) / deltaPos;

    return finalPercent;
}

void TpScrollPanel::setVerticalScrollBarValue(const uint32_t &value)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return;

    TpScroll *scrollBar = set->scroll[TP_RBSCR_INDEX_V];

    uint32_t deltaPos = scrollBar->max() - scrollBar->min();
    uint32_t newPosion = value * deltaPos / 100.0;

    scrollBar->setPosition(newPosion);

    if (set->centralWidget)
    {
        // std::cout << "set->centralWidget Height  " << set->centralWidget->height() << std::endl;

        int32_t x = set->centralWidget->pos().x();
        int32_t y = set->centralWidget->pos().y();

        set->centralWidget->move(x, -newPosion);
        set->centralWidget->broadSetTop();
    }
    else
    {
        int32_t x = set->panel->pos().x();
        int32_t y = set->panel->pos().y();

        set->panel->move(x, -newPosion);
        set->panel->broadSetTop();
    }

    TpChildWidget::update();
}

int32_t TpScrollPanel::horizontalPostion()
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return 0;

    if (set->centralWidget)
        return set->centralWidget->pos().x();

    return set->panel->pos().x();
}

void TpScrollPanel::setHorizontalPostion(const int32_t &value)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return;

    TpScroll *scrollBar = set->scroll[TP_LTSCR_INDEX_H];
    scrollBar->setPosition(-value);

    if (set->centralWidget)
    {
        int32_t x = set->centralWidget->pos().x();
        int32_t y = set->centralWidget->pos().y();

        set->centralWidget->move(value, y);
        set->centralWidget->broadSetTop();
    }
    else
    {
        int32_t x = set->panel->pos().x();
        int32_t y = set->panel->pos().y();

        set->panel->move(value, y);
        set->panel->broadSetTop();
    }

    TpChildWidget::update();
}

int32_t TpScrollPanel::verticalPostion()
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return 0;

    if (set->centralWidget)
        return set->centralWidget->pos().y();

    return set->panel->pos().y();
}

void TpScrollPanel::setVerticalPostion(const int32_t &value)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return;

    TpScroll *scrollBar = set->scroll[TP_RBSCR_INDEX_V];
    scrollBar->setPosition(-value);

    if (set->centralWidget)
    {
        int32_t x = set->centralWidget->pos().x();
        int32_t y = set->centralWidget->pos().y();

        set->centralWidget->move(x, value);
        set->centralWidget->broadSetTop();
    }
    else
    {
        int32_t x = set->panel->pos().x();
        int32_t y = set->panel->pos().y();

        set->panel->move(x, value);
        set->panel->broadSetTop();
    }

    update();
}

bool TpScrollPanel::scrollMode()
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return false;

    return set->isVertical;
}

void TpScrollPanel::setScrollMode(const bool &isVertical)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return;

    set->isVertical = isVertical;
}

void TpScrollPanel::setVerticalScrollBarVisible(const bool &visible)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return;

    set->scroll[TP_RBSCR_INDEX_V]->setVisible(visible);
    set->VScrollBarVisible = visible;
}

void TpScrollPanel::setHorizontalScrollBarVisible(const bool &visible)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return;

    set->scroll[TP_LTSCR_INDEX_H]->setVisible(visible);
    set->HScrollBarVisible = visible;
}

bool TpScrollPanel::addObject(TpChildWidget *object)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
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

bool TpScrollPanel::delObject(TpChildWidget *object)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
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

    TpRect rect = set->objectRect[object];
    object->setRect(rect);

    auto mapIter = set->objectRect.find(object);

    if (mapIter != set->objectRect.end())
    {
        set->objectRect.erase(mapIter);
    }

    return ret;
}

bool TpScrollPanel::setWidget(TpChildWidget *widget)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
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

TpChildWidget *TpScrollPanel::widget()
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    return set->centralWidget;
}

TpVector<TpChildWidget *> TpScrollPanel::children()
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    return set->objectList;
}

bool TpScrollPanel::clearObject()
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);

    if (!set)
        return false;

    for (auto iter = set->objectList.begin(); iter != set->objectList.end(); iter++)
    {
        TpRect rect = set->objectRect[*iter];
        (*iter)->setRect(rect);

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

bool TpScrollPanel::recal(bool enableOffset)
{
    // std::cout << " TpScrollPanel::recal(bool enableOffset) " << std::endl;
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
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

        TpPoint tltPoint, trbPoint;

        tltPoint.setX(set->objectList.at(0)->rect().x());
        tltPoint.setY(set->objectList.at(0)->rect().y());

        trbPoint.setX(set->objectList.at(0)->rect().x() + set->objectList.at(0)->rect().width());
        trbPoint.setY(set->objectList.at(0)->rect().y() + set->objectList.at(0)->rect().height());

        for (int32_t i = 1; i < size; i++)
        {
            tltPoint.setX(TP_MIN(tltPoint.x(), set->objectList.at(i)->pos().x()));
            tltPoint.setY(TP_MIN(tltPoint.y(), set->objectList.at(i)->pos().y()));

            trbPoint.setX(TP_MAX(trbPoint.x(), set->objectList.at(i)->pos().x() + set->objectList.at(i)->width()));
            trbPoint.setY(TP_MAX(trbPoint.x(), set->objectList.at(i)->pos().y() + set->objectList.at(i)->height()));
            set->objectRect[set->objectList.at(i)] = set->objectList.at(i)->rect();
        }

        set->ltPoint = tltPoint;
        set->rbPoint = trbPoint;

        uint32_t firstObjectX = set->ltPoint.x();
        uint32_t firstObjectY = set->ltPoint.y();

        uint32_t realWidth = set->rbPoint.x() - set->ltPoint.x();
        uint32_t realHeight = set->rbPoint.y() - set->ltPoint.y();

        if (set->ltPoint.x() < 0)
        {
            set->offsetX = TP_ABS(set->ltPoint.x());
        }

        if (set->ltPoint.y() < 0)
        {
            set->offsetY = TP_ABS(set->ltPoint.y());
        }

        if (realWidth < rectWidth)
        {
            realWidth = rectWidth;
        }

        if (realHeight < rectHeight)
        {
            realHeight = rectHeight;
        }

        if (set->ltPoint.x() > 0)
        {
            set->ltPoint.setX(0);
        }

        if (set->ltPoint.y() > 0)
        {
            set->ltPoint.setY(0);
        }

        // 最后panel的宽度高度要叠加上第一个成员的X偏移量 WHY
        // uint32_t acutalPanelWidth = realWidth + firstObjectX;
        // uint32_t acutalPanelHeight = realHeight + firstObjectY;
        realWidth += firstObjectX;
        realHeight += firstObjectY;

        int32_t scrollAreaX = realWidth - rectWidth;
        int32_t scrollAreaY = realHeight - rectHeight;

        set->panel->setRect(0, 0, realWidth, realHeight);
        // std::cout << "acutalPanelWidth " << acutalPanelWidth << "  realHeight  " << realHeight << std::endl;

        if (enableOffset)
        {
            auto iter = set->objectList.begin();

            for (; iter != set->objectList.end(); iter++)
            {
                (*iter)->move(set->offsetX + (*iter)->pos().x(), set->offsetY + (*iter)->pos().y());
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

void TpScrollPanel::doFlip(TpScroll *scroll, int32_t position, int32_t page, int32_t pageIndex, double percent)
{
    std::cout << "TpScrollPanel::doFlip " << std::endl;

    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    int32_t x = set->panel->pos().x();
    int32_t y = set->panel->pos().y();

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
    TpChildWidget::update();

    // set->scroll[TP_LTSCR_INDEX]
}

bool TpScrollPanel::onPaintEvent(TpPaintEvent *event)
{
    TpChildWidget::onPaintEvent(event);
    // recal(true);
    return true;
}

bool TpScrollPanel::eventFilter(TpObject *watched, TpEvent *event)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return false;

    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseKeyEvent = dynamic_cast<TpMouseEvent *>(event);
        if (!mouseKeyEvent)
            return false;

        onMousePressEvent(mouseKeyEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpMouseEvent *mouseKeyEvent = dynamic_cast<TpMouseEvent *>(event);
        if (!mouseKeyEvent)
            return false;

        onMouseRleaseEvent(mouseKeyEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_MOVE_TYPE)
    {
        TpMouseEvent *mouseMotionEvent = dynamic_cast<TpMouseEvent *>(event);
        if (!mouseMotionEvent)
            return false;

        // std::cout << "TpScrollPanel::eventFilter  Move" << std::endl;
        onMouseMoveEvent(mouseMotionEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_WHEEL_EVENT)
    {
        TpWheelEvent *wheelEvent = dynamic_cast<TpWheelEvent *>(event);
        if (!wheelEvent)
            return false;

        onWheelEvent(wheelEvent);
    }
    else
    {
    }

    if (set->centralWidget == watched)
    {
        if (event->eventType() == TpEvent::EVENT_OBJECT_RESIZE_TYPE)
        {
            recal();
        }
    }

    return false;
}

bool TpScrollPanel::onMousePressEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return false;

    // 鼠标左键点击，记录点击坐标
    set->mouseLeftPress = event->state();
    set->originPressPoint_ = event->globalPos();
    set->updatePoint_ = set->originPressPoint_;

    return true;
}

bool TpScrollPanel::onMouseRleaseEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
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

bool TpScrollPanel::onMouseMoveEvent(TpMouseEvent *event)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return false;

    if (set->mouseLeftPress)
    {
        TpPoint curPoint = event->globalPos();

        int32_t offset = 0;
        if (scrollMode())
        {
            // 纵向
            offset = curPoint.y() - set->updatePoint_.y();

            // std::cout << "******* this  " << this << std::endl;
            // std::cout << " *******curPoint.y " << curPoint.y << "   set->updatePoint_.y " << set->updatePoint_.y << std::endl;
            // std::cout << " *******offset " << offset << std::endl;
            // std::cout << "******verticalPostion() " << verticalPostion() << std::endl;

            setVerticalPostion(verticalPostion() + offset);
        }
        else
        {
            // 横向
            offset = curPoint.x() - set->updatePoint_.x();

            // std::cout << "horizontalPostion() " << horizontalPostion() << " offset" << std::endl;
            setHorizontalPostion(horizontalPostion() + offset);
        }

        set->updatePoint_ = curPoint;

        // TpChildWidget::update();
    }

    return true;
}

bool TpScrollPanel::onWheelEvent(TpWheelEvent *event)
{
    int32_t delta = event->angleDelta();

    if (delta > 0)
    {
        // 捕获鼠标滚轮事件,

        int32_t curScrollValue = scrollMode() ? verticalScrollBarValue() : horizontalScrollBarValue();
        // std::cout << "curScrollValue : " << curScrollValue << std::endl;

        curScrollValue -= WHEEL_SCROLL_INTERVAL;

        if (curScrollValue < 0)
            curScrollValue = 0;

        scrollMode() ? setVerticalScrollBarValue(curScrollValue) : setHorizontalScrollBarValue(curScrollValue);
    }
    else
    {
        int32_t curScrollValue = scrollMode() ? verticalScrollBarValue() : horizontalScrollBarValue();
        // std::cout << "curScrollValue : " << curScrollValue << std::endl;

        curScrollValue += WHEEL_SCROLL_INTERVAL;

        if (curScrollValue > 100)
            curScrollValue = 100;

        scrollMode() ? setVerticalScrollBarValue(curScrollValue) : setHorizontalScrollBarValue(curScrollValue);
    }

    return true;
}

bool TpScrollPanel::onResizeEvent(TpResizeEvent *event)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
    if (!set)
        return true;

    if (set->centralWidget)
    {
        set->centralWidget->setRect(0, 0, width(), height());
    }
    recal();

    return true;
}

bool TpScrollPanel::onLeaveEvent(TpLeaveEvent *event)
{
    TpScrollPanelData *set = static_cast<TpScrollPanelData *>(data_);
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
