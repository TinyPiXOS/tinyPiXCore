#include "MainAppScrollPanel.h"
#include "DeskTopGlobal.hpp"
#include "math.h"

MainAppScrollPanel::MainAppScrollPanel(TpWidget *parent)
    : TpScrollPanel(parent), maxPageCount_(globalDesktopMaxPageNum), isSwitchPage_(false)
{
    setEnableBackGroundColor(false);

    TP_PROPERTY(int32_t, horizontalPostion, horizontalPostion, setHorizontalPostion);

    valueAnimation_ = new TpAnimation(this, "horizontalPostion");
    valueAnimation_->setDuration(200);
}

MainAppScrollPanel::~MainAppScrollPanel()
{
}

void MainAppScrollPanel::setMaxPage(const uint32_t &maxPage)
{
    maxPageCount_ = maxPage;
}

void MainAppScrollPanel::setPage(const uint32_t &page)
{
}

void MainAppScrollPanel::setMoveAppRect(const TpRect &rect, const int32_t &round)
{
}

void MainAppScrollPanel::setAnimalHorizontalPostion(int32_t value)
{
    if (horizontalPostion() == value)
        return;

    valueAnimation_->stop();
    valueAnimation_->setStartValue(horizontalPostion());
    valueAnimation_->setEndValue(value);
    valueAnimation_->start(TpAnimation::KeepWhenStopped);
}

bool MainAppScrollPanel::eventFilter(TpObject *watched, TpEvent *event)
{
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

        onMouseMoveEvent(mouseMotionEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_WHEEL_EVENT)
    {
        TpWheelEvent *wheelEvent = dynamic_cast<TpWheelEvent *>(event);
        if (!wheelEvent)
            return false;

        onWheelEvent(wheelEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_LONG_PRESS_TYPE)
    {
        TpMouseEvent *mouseKeyEvent = dynamic_cast<TpMouseEvent *>(event);
        if (!mouseKeyEvent)
            return false;

        onMouseLongPressEvent(mouseKeyEvent);
    }
    else
    {
    }

    return false;
}

bool MainAppScrollPanel::onMousePressEvent(TpMouseEvent *event)
{
    TpScrollPanel::onMousePressEvent(event);

    // 鼠标左键点击，记录点击坐标
    originPressPoint_ = event->globalPos();
    updatePoint_ = originPressPoint_;
    originPressPos_ = horizontalPostion();
    // std::cout << "event->globalPos().X: " << event->globalPos().x() << "  event->globalPos().y(): " << event->globalPos().y() << std::endl;

    valueAnimation_->stop();

    return true;
}

bool MainAppScrollPanel::onMouseRleaseEvent(TpMouseEvent *event)
{
    // TpScrollPanel::onMouseRleaseEvent(event);

    int32_t curScrollValue = horizontalPostion();

    if (curScrollValue == 0 || (curScrollValue % rect().width() == 0))
        return true;

    // 计算移动偏移量，决定是否翻页

    // std::cout << "release curScrollValue : " << curScrollValue << std::endl;
    // 判断当前偏移量距离哪一页近，就翻到哪一页
    int32_t nextPageValue = -rect().width();
    int32_t prePageValue = 0;

    // 找到当前偏移量的上一页和下一页偏移量 prePageValue > nextPageValue
    while (nextPageValue > curScrollValue)
    {
        nextPageValue -= rect().width();
    }
    prePageValue = nextPageValue + rect().width();

    if (prePageValue > 0)
        prePageValue = 0;

    int32_t minValue = -(rect().width() * (maxPageCount_ - 1));
    if (nextPageValue < minValue)
        nextPageValue = minValue;

    int32_t curPostion = isLeftRoll_ ? nextPageValue : prePageValue;

    setAnimalHorizontalPostion(curPostion);

    onPageChanged.emit(std::fabs(curPostion) / rect().width());

    return true;
}

bool MainAppScrollPanel::onMouseLongPressEvent(TpMouseEvent *event)
{
    TpScrollPanel::onMouseLongPressEvent(event);

    onLongPress.emit();

    return false;
}

bool MainAppScrollPanel::onWheelEvent(TpWheelEvent *event)
{
    if (event->angleDelta() > 0)
    {
        int32_t curScrollValue = horizontalPostion();

        curScrollValue += rect().width();

        if (curScrollValue > 0)
            curScrollValue = 0;

        setAnimalHorizontalPostion(curScrollValue);

        onPageChanged.emit(std::fabs(curScrollValue) / rect().width());
    }
    else
    {
        int32_t curScrollValue = horizontalPostion();

        curScrollValue -= rect().width();

        int32_t minValue = -(rect().width() * (maxPageCount_ - 1));
        if (curScrollValue < minValue)
            curScrollValue = minValue;

        setAnimalHorizontalPostion(curScrollValue);

        onPageChanged.emit(std::fabs(curScrollValue) / rect().width());
    }

    return true;
}

bool MainAppScrollPanel::onMouseMoveEvent(TpMouseEvent *event)
{
    // TpScrollPanel::onMouseMoveEvent(event);

    if (event->state())
    {
        TpPoint curPoint = event->globalPos();

        // 如果是横屏，只计算X偏移量，竖屏则只计算Y偏移量
        int32_t curScrollValue = horizontalPostion();

        int32_t offset = curPoint.x() - updatePoint_.x();

        updatePoint_ = curPoint;

        {
            int32_t isLeftOffset = curPoint.x() - originPressPoint_.x();
            isLeftRoll_ = isLeftOffset > 0 ? false : true;
        }

        // 偏移超过5像素才翻页
        isSwitchPage_ = std::abs(curPoint.x() - originPressPoint_.x()) > 50 ? true : false;

        curScrollValue = curScrollValue + offset;

        int32_t minValue = -(rect().width() * (maxPageCount_ - 1));

        if (curScrollValue > 0)
            curScrollValue = 0;

        if (curScrollValue < minValue)
            curScrollValue = minValue;

        setHorizontalPostion(curScrollValue);
    }

    return true;
}

bool MainAppScrollPanel::onLeaveEvent(TpLeaveEvent *event)
{
    // TpScrollPanel::onLeaveEvent(event);

    return true;
}
