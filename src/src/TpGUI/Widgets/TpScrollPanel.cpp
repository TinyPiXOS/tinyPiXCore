#include "TpScrollPanel.h"
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
    int32_t scrollRange[TP_SCROLL_NUM];

    TpWidget *panel;
    TpVector<TpWidget *> objectList;
    TpMap<TpWidget *, TpRect> objectRect;

    // 滚动窗口
    TpWidget *centralWidget = nullptr;

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

TpScrollPanel::TpScrollPanel(TpWidget *parent)
    : TpWidget(parent)
{
    TpScrollPanelData *scrollData = new TpScrollPanelData();

    if (!scrollData)
        return;

    scrollData->panel = new TpWidget(this);
    scrollData->panel->setEnableBackGroundColor(true);
    scrollData->panel->setEnableBackGroundImage(false);
    scrollData->panel->setEnabledBorderColor(false);
    scrollData->panel->setVisible(true);

    scrollData->panel->installEventFilter(this);

    scrollData->ltPoint.setX(INT_MAX);
    scrollData->ltPoint.setY(INT_MAX);

    scrollData->rbPoint.setX(INT_MIN);
    scrollData->rbPoint.setY(INT_MIN);

    scrollData->offsetX = 0;
    scrollData->offsetY = 0;

    data_ = scrollData;

    setVisible(true);
}

TpScrollPanel::~TpScrollPanel()
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);

    if (!scrollData)
        return;

    if (scrollData->panel)
    {
        delete scrollData->panel;
        scrollData->panel = nullptr;
    }

    delete scrollData;
    scrollData = nullptr;
}

uint32_t TpScrollPanel::horizontalScrollBarValue()
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return 0;

    uint32_t deltaPos = scrollData->scrollRange[TP_LTSCR_INDEX_H];
    if (deltaPos == 0)
        return 0;

    uint32_t finalPercent = 100.0 * std::abs(horizontalPostion()) / deltaPos;

    return finalPercent;
}

void TpScrollPanel::setHorizontalScrollBarValue(const uint32_t &value)
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return;

    uint32_t newPosion = value * scrollData->scrollRange[TP_LTSCR_INDEX_H] / 100.0;

    if (scrollData->centralWidget)
    {
        int32_t x = scrollData->centralWidget->pos().x();
        int32_t y = scrollData->centralWidget->pos().y();

        scrollData->centralWidget->move(-newPosion, y);
        scrollData->centralWidget->broadSetTop();
    }
    else
    {
        int32_t x = scrollData->panel->pos().x();
        int32_t y = scrollData->panel->pos().y();

        scrollData->panel->move(-newPosion, y);
        scrollData->panel->broadSetTop();
    }

    TpWidget::update();
}

uint32_t TpScrollPanel::verticalScrollBarValue()
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return 0;

    uint32_t deltaPos = scrollData->scrollRange[TP_RBSCR_INDEX_V];
    if (deltaPos == 0)
        return 0;

    uint32_t finalPercent = 100.0 * std::abs(verticalPostion()) / deltaPos;

    return finalPercent;
}

void TpScrollPanel::setVerticalScrollBarValue(const uint32_t &value)
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return;

    uint32_t newPosion = value * scrollData->scrollRange[TP_RBSCR_INDEX_V] / 100.0;

    if (scrollData->centralWidget)
    {
        // std::cout << "scrollData->centralWidget Height  " << scrollData->centralWidget->height() << std::endl;

        int32_t x = scrollData->centralWidget->pos().x();
        int32_t y = scrollData->centralWidget->pos().y();

        scrollData->centralWidget->move(x, -newPosion);
        scrollData->centralWidget->broadSetTop();
    }
    else
    {
        int32_t x = scrollData->panel->pos().x();
        int32_t y = scrollData->panel->pos().y();

        scrollData->panel->move(x, -newPosion);
        scrollData->panel->broadSetTop();
    }

    TpWidget::update();
}

int32_t TpScrollPanel::horizontalPostion()
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return 0;

    if (scrollData->centralWidget)
        return scrollData->centralWidget->pos().x();

    return scrollData->panel->pos().x();
}

void TpScrollPanel::setHorizontalPostion(const int32_t &value)
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return;

    if (scrollData->centralWidget)
    {
        int32_t x = scrollData->centralWidget->pos().x();
        int32_t y = scrollData->centralWidget->pos().y();

        scrollData->centralWidget->move(value, y);
        scrollData->centralWidget->broadSetTop();
    }
    else
    {
        int32_t x = scrollData->panel->pos().x();
        int32_t y = scrollData->panel->pos().y();

        scrollData->panel->move(value, y);
        scrollData->panel->broadSetTop();
    }

    TpWidget::update();
}

int32_t TpScrollPanel::verticalPostion()
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return 0;

    if (scrollData->centralWidget)
        return scrollData->centralWidget->pos().y();

    return scrollData->panel->pos().y();
}

void TpScrollPanel::setVerticalPostion(const int32_t &value)
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return;

    if (scrollData->centralWidget)
    {
        int32_t x = scrollData->centralWidget->pos().x();
        int32_t y = scrollData->centralWidget->pos().y();

        scrollData->centralWidget->move(x, value);
        scrollData->centralWidget->broadSetTop();
    }
    else
    {
        int32_t x = scrollData->panel->pos().x();
        int32_t y = scrollData->panel->pos().y();

        scrollData->panel->move(x, value);
        scrollData->panel->broadSetTop();
    }

    update();
}

bool TpScrollPanel::scrollMode()
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return false;

    return scrollData->isVertical;
}

void TpScrollPanel::setScrollMode(const bool &isVertical)
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return;

    scrollData->isVertical = isVertical;
}

bool TpScrollPanel::addObject(TpWidget *object)
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    bool ret = false;

    if (!scrollData)
        return ret;

    object->setParent(scrollData->panel);
    object->installEventFilter(this);
    ret = (object->parent() == scrollData->panel);

    if (ret)
    {
        scrollData->objectList.push_back(object);
    }

    recal();

    return ret;
}

bool TpScrollPanel::delObject(TpWidget *object)
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    bool ret = false;

    if (!scrollData)
        return ret;

    object->setParent(nullptr);
    object->uninstallEventFilter();
    ret = (object->parent() == nullptr);

    if (!ret)
        return ret;

    auto iter = scrollData->objectList.begin();

    for (; iter != scrollData->objectList.end(); iter++)
    {
        if (*iter == object)
        {
            break;
        }
    }

    if (iter != scrollData->objectList.end())
    {
        scrollData->objectList.erase(iter);
    }

    TpRect rect = scrollData->objectRect[object];
    object->setRect(rect);

    auto mapIter = scrollData->objectRect.find(object);

    if (mapIter != scrollData->objectRect.end())
    {
        scrollData->objectRect.erase(mapIter);
    }

    return ret;
}

bool TpScrollPanel::setWidget(TpWidget *widget)
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return false;

    if (scrollData->centralWidget)
    {
        scrollData->centralWidget->uninstallEventFilter();
        scrollData->centralWidget->setParent(nullptr);
    }

    // addObject(widget);
    scrollData->centralWidget = widget;
    // scrollData->centralWidget->setBackGroundColor(_RGB(0, 255, 0));

    if (widget)
    {
        widget->installEventFilter(this);
        widget->setParent(this);

        // widget->setRect(0, 0, scrollData->panel->width(), scrollData->panel->height());
        widget->setRect(0, 0, width(), height());
        recal();

        // std::cout << "SetWidget  :: Height  " << height() << std::endl;
    }

    update();

    return true;
}

TpWidget *TpScrollPanel::widget()
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    return scrollData->centralWidget;
}

TpVector<TpWidget *> TpScrollPanel::children()
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    return scrollData->objectList;
}

bool TpScrollPanel::clearObject()
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);

    if (!scrollData)
        return false;

    for (auto iter = scrollData->objectList.begin(); iter != scrollData->objectList.end(); iter++)
    {
        auto mapIter = scrollData->objectRect.find(*iter);

        if (mapIter != scrollData->objectRect.end())
        {
            TpRect rect = scrollData->objectRect[*iter];
            (*iter)->setRect(rect);
            scrollData->objectRect.erase(mapIter);
        }
    }

    scrollData->objectList.clear();
    recal();

    return true;
}

bool TpScrollPanel::recal(bool enableOffset)
{
    // std::cout << " TpScrollPanel::recal(bool enableOffset) " << std::endl;
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    bool ret = false;

    if (!scrollData)
        return false;

    uint32_t rectWidth = width();
    uint32_t rectHeight = height();

    if (scrollData->centralWidget)
    {
        uint32_t realWidth = scrollData->centralWidget->width();
        uint32_t realHeight = scrollData->centralWidget->height();

        if (realWidth < rectWidth)
            realWidth = rectWidth;

        if (realHeight < rectHeight)
            realHeight = rectHeight;

        int32_t scrollAreaX = realWidth - rectWidth;
        int32_t scrollAreaY = realHeight - rectHeight;

        if (realWidth >= rectWidth)
        {
            scrollData->scrollRange[TP_LTSCR_INDEX_H] = scrollAreaX;
            realWidth -= 5;
        }

        if (realHeight >= rectHeight)
        {
            scrollData->scrollRange[TP_RBSCR_INDEX_V] = scrollAreaY;
            realHeight -= 5;
        }
    }
    else
    {
        int32_t size = scrollData->objectList.size();

        if (size == 0)
            return false;

        TpPoint tltPoint, trbPoint;

        tltPoint.setX(scrollData->objectList.at(0)->rect().x());
        tltPoint.setY(scrollData->objectList.at(0)->rect().y());

        trbPoint.setX(scrollData->objectList.at(0)->rect().x() + scrollData->objectList.at(0)->rect().width());
        trbPoint.setY(scrollData->objectList.at(0)->rect().y() + scrollData->objectList.at(0)->rect().height());

        for (int32_t i = 1; i < size; i++)
        {
            tltPoint.setX(TP_MIN(tltPoint.x(), scrollData->objectList.at(i)->pos().x()));
            tltPoint.setY(TP_MIN(tltPoint.y(), scrollData->objectList.at(i)->pos().y()));

            trbPoint.setX(TP_MAX(trbPoint.x(), scrollData->objectList.at(i)->pos().x() + scrollData->objectList.at(i)->width()));
            trbPoint.setY(TP_MAX(trbPoint.x(), scrollData->objectList.at(i)->pos().y() + scrollData->objectList.at(i)->height()));
            scrollData->objectRect[scrollData->objectList.at(i)] = scrollData->objectList.at(i)->rect();
        }

        scrollData->ltPoint = tltPoint;
        scrollData->rbPoint = trbPoint;

        uint32_t firstObjectX = scrollData->ltPoint.x();
        uint32_t firstObjectY = scrollData->ltPoint.y();

        uint32_t realWidth = scrollData->rbPoint.x() - scrollData->ltPoint.x();
        uint32_t realHeight = scrollData->rbPoint.y() - scrollData->ltPoint.y();

        if (scrollData->ltPoint.x() < 0)
        {
            scrollData->offsetX = TP_ABS(scrollData->ltPoint.x());
        }

        if (scrollData->ltPoint.y() < 0)
        {
            scrollData->offsetY = TP_ABS(scrollData->ltPoint.y());
        }

        if (realWidth < rectWidth)
        {
            realWidth = rectWidth;
        }

        if (realHeight < rectHeight)
        {
            realHeight = rectHeight;
        }

        if (scrollData->ltPoint.x() > 0)
        {
            scrollData->ltPoint.setX(0);
        }

        if (scrollData->ltPoint.y() > 0)
        {
            scrollData->ltPoint.setY(0);
        }

        // 最后panel的宽度高度要叠加上第一个成员的X偏移量 WHY
        // uint32_t acutalPanelWidth = realWidth + firstObjectX;
        // uint32_t acutalPanelHeight = realHeight + firstObjectY;
        realWidth += firstObjectX;
        realHeight += firstObjectY;

        int32_t scrollAreaX = realWidth - rectWidth;
        int32_t scrollAreaY = realHeight - rectHeight;

        scrollData->panel->setRect(0, 0, realWidth, realHeight);

        if (enableOffset)
        {
            auto iter = scrollData->objectList.begin();

            for (; iter != scrollData->objectList.end(); iter++)
            {
                (*iter)->move(scrollData->offsetX + (*iter)->pos().x(), scrollData->offsetY + (*iter)->pos().y());
            }
        }

        if (realWidth >= rectWidth)
        {
            scrollData->scrollRange[TP_LTSCR_INDEX_H] = scrollAreaX;
            realWidth -= 5;
        }

        if (realHeight >= rectHeight)
        {
            scrollData->scrollRange[TP_RBSCR_INDEX_V] = scrollAreaY;
            realHeight -= 5;
        }
    }

    // update();

    return ret;
}

bool TpScrollPanel::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);
    // recal(true);
    return true;
}

bool TpScrollPanel::eventFilter(TpObject *watched, TpEvent *event)
{
    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
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

    if (scrollData->centralWidget == watched)
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
    TpWidget::onMousePressEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return false;

    // 鼠标左键点击，记录点击坐标
    scrollData->mouseLeftPress = event->state();
    scrollData->originPressPoint_ = event->globalPos();
    scrollData->updatePoint_ = scrollData->originPressPoint_;

    return true;
}

bool TpScrollPanel::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpWidget::onMouseRleaseEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return false;

    scrollData->mouseLeftPress = event->state();

    // std::cout << "scrollData->centralWidget->height(): " << scrollData->centralWidget->height() << std::endl;

    int32_t curScrollValue = scrollMode() ? verticalPostion() : horizontalPostion();

    if (curScrollValue > 0)
    {
        curScrollValue = 0;
        scrollMode() ? setVerticalPostion(curScrollValue) : setHorizontalPostion(curScrollValue);
    }
    else
    {
        uint32_t delta = 0;

        if (scrollMode())
        {
            delta = scrollData->scrollRange[TP_RBSCR_INDEX_V];
        }
        else
        {
            delta = scrollData->scrollRange[TP_LTSCR_INDEX_H];
        }

        if (std::fabs(curScrollValue) > delta)
        {
            curScrollValue = -delta;
            scrollMode() ? setVerticalPostion(curScrollValue) : setHorizontalPostion(curScrollValue);
        }
    }

    // 鼠标释放，如果拖拽的偏移边界了，回归0点位或终止位
    // int32_t curScrollValue = scrollMode() ? verticalScrollBarValue() : horizontalScrollBarValue();
    // if (curScrollValue <= 0)
    // {
    //     curScrollValue = 0;
    //     scrollMode() ? setVerticalScrollBarValue(curScrollValue) : setHorizontalScrollBarValue(curScrollValue);
    // }
    // else if (curScrollValue >= 100)
    // {
    //     curScrollValue = 100;
    //     scrollMode() ? setVerticalScrollBarValue(curScrollValue) : setHorizontalScrollBarValue(curScrollValue);
    // }
    // else
    // {
    // }

    return true;
}

bool TpScrollPanel::onMouseMoveEvent(TpMouseEvent *event)
{
    TpWidget::onMouseMoveEvent(event);

    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return false;

    if (scrollData->mouseLeftPress)
    {
        TpPoint curPoint = event->globalPos();

        int32_t offset = 0;
        if (scrollMode())
        {
            // 纵向
            offset = curPoint.y() - scrollData->updatePoint_.y();

            // std::cout << "******* this  " << this << std::endl;
            // std::cout << " *******curPoint.y " << curPoint.y << "   scrollData->updatePoint_.y " << scrollData->updatePoint_.y << std::endl;
            // std::cout << " *******offset " << offset << std::endl;
            // std::cout << "******verticalPostion() " << verticalPostion() << std::endl;

            setVerticalPostion(verticalPostion() + offset);
        }
        else
        {
            // 横向
            offset = curPoint.x() - scrollData->updatePoint_.x();

            // std::cout << "horizontalPostion() " << horizontalPostion() << " offset" << std::endl;
            setHorizontalPostion(horizontalPostion() + offset);
        }

        scrollData->updatePoint_ = curPoint;

        // TpWidget::update();
    }

    return true;
}

bool TpScrollPanel::onWheelEvent(TpWheelEvent *event)
{
    TpWidget::onWheelEvent(event);

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
    TpWidget::onResizeEvent(event);

    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return true;

    if (scrollData->centralWidget)
    {
        scrollData->centralWidget->setRect(0, 0, width(), height());
    }
    recal();

    return true;
}

bool TpScrollPanel::onLeaveEvent(TpLeaveEvent *event)
{
    TpWidget::onLeaveEvent(event);

    TpScrollPanelData *scrollData = static_cast<TpScrollPanelData *>(data_);
    if (!scrollData)
        return true;

    // std::cout << "event->leave() " << event->leave() << std::endl;
    if (!event->leave())
    {
        scrollData->mouseLeftPress = false;

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
