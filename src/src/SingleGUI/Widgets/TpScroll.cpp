#include "TpScroll.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "TpRange.h"
#include "TpRect.h"
#include <cstring>

typedef struct
{
    TpRange range;
    TpRange rectRange;
    TpRange mapRange;

    struct
    {
        bool direction;
        bool down;
    };

    TpRect barRect;

    struct
    {
        int32_t normalBarColor;
        int32_t hoverBarColor;
        int32_t downBarColor;
    };

    struct
    {
        int32_t lineColor;
        int32_t barColor;
    };

    int32_t lines;
    int32_t delta;
    int32_t lastBarPos;
} ItpScrollSet;

static inline void adjustBar(TpScroll *scroll, ItpScrollSet *set)
{
    double percent = set->range.percent();
    int32_t barW = TP_MIN(scroll->width(), scroll->height()) - 2;
    int32_t barLength = (int32_t)(set->lines * set->rectRange.length() / (double)set->range.length());

    if (barLength < TP_MIN(scroll->width() - 1, scroll->height() - 1))
    {
        barLength = TP_MIN(scroll->width() - 1, scroll->height() - 1);
    }

    switch (set->direction)
    {
    case false:
    {
        set->barRect.setX(percent * set->rectRange.length());

        if ((set->barRect.x() + barLength) > (scroll->width() - 1))
        {
            set->barRect.setX(scroll->width() - barLength - 1);
        }

        if (set->barRect.x() < 1)
        {
            set->barRect.setX(1);
        }

        set->barRect.setY(1);
        set->barRect.setWidth(barLength);
        set->barRect.setHeight(barW);

        set->mapRange.setRange(1, scroll->width() - barLength - 1);
    }
    break;
    case true:
    {
        set->barRect.setY(percent * set->rectRange.length());

        if ((set->barRect.y() + barLength) > (scroll->height() - 1))
        {
            set->barRect.setY(scroll->height() - barLength - 1);
        }

        if (set->barRect.y() < 1)
        {
            set->barRect.setY(1);
        }

        set->barRect.setX(1);
        set->barRect.setWidth(barW);
        set->barRect.setHeight(barLength);

        set->mapRange.setRange(1, scroll->height() - barLength - 1);
    }
    break;
    }

    set->mapRange.setPercent(percent);
}

TpScroll::TpScroll(TpChildWidget *parent, bool horizontal) : TpChildWidget(parent)
{
    ItpScrollSet *set = new ItpScrollSet();

    if (set)
    {
        set->range.setRange(0, 100);
        set->rectRange.setRange(0, 100);
        set->mapRange.setRange(1, 99);
        set->direction = horizontal;
        set->down = false;
        set->lineColor = TpColors::White;
        set->lines = 20;
        set->delta = 0;
        set->lastBarPos = 1;

        set->normalBarColor = TpColors::LightGray;
        set->hoverBarColor = TpColors::Grey;
        set->downBarColor = TpColors::DarkGoldenrod;
        set->barColor = TpColors::LightGray;

        this->setBackGroundColor(TpColors::White);
        this->scrollSet = set;
    }
}

TpScroll::~TpScroll()
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        delete set;
    }
}

bool TpScroll::direction()
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    bool direction = false;

    if (set)
    {
        direction = set->direction;
    }

    return direction;
}

void TpScroll::setLinePerPage(int32_t lines)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        if (lines <= 0)
        {
            lines = 1;
        }

        if (lines >= set->range.length())
        {
            lines = set->range.length();
        }

        set->lines = lines;
        adjustBar(this, set);
    }
}

int32_t TpScroll::pages()
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    int32_t page = 0;

    if (set)
    {
        int32_t mod = set->range.length() % set->lines;
        page = set->range.length() / set->lines;

        if (mod)
        {
            page++;
        }
    }

    return page;
}

int32_t TpScroll::pageIndex()
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    int32_t index = 0;

    if (set)
    {
        int32_t mod = set->range.length() % set->lines;
        int32_t page = set->range.length() / set->lines - 1;

        if (mod)
        {
            page++;
        }

        index = (int32_t)(page * set->range.percent());
    }

    return index;
}

int32_t TpScroll::linesPerPage()
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    if (!set)
        return 0;

    return set->lines;
}

int32_t TpScroll::min()
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    if (!set)
        return 0;

    return set->range.min();
}

int32_t TpScroll::max()
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    if (!set)
        return 0;

    return set->range.max();
}

void TpScroll::setMaxRange(int32_t max)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        if (max < 0)
        {
            max = 0;
        }

        double percent = set->range.percent();
        set->range.setRange(0, max);
        set->range.setPercent(percent);
        set->rectRange.setPercent(percent);

        adjustBar(this, set);
    }
}

void TpScroll::zoomRange(int32_t delta)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        int32_t max = set->range.max() + delta;

        if (max < 0)
        {
            max = 0;
        }

        double percent = set->range.percent();
        set->range.setRange(0, max);
        set->range.setPercent(percent);
        set->rectRange.setPercent(percent);

        adjustBar(this, set);
    }
}

void TpScroll::setDirection(bool horizontal)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        set->direction = horizontal;
        int32_t w = this->width(), h = this->height();

        switch (set->direction)
        {
        case false:
        {
            set->rectRange.setRange(0, w);
        }
        break;
        case true:
        {
            set->rectRange.setRange(0, h);
        }
        break;
        }

        adjustBar(this, set);
    }
}

void TpScroll::setPosition(int32_t position)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (!set)
        return;

    if (position < set->range.min())
    {
        position = set->range.min();
    }

    if (position > set->range.max())
    {
        position = set->range.max();
    }

    if (position == set->range.position())
    {
        return;
    }

    set->range.setPosition(position);
    double percent = set->range.percent();
    set->rectRange.setPercent(percent);
    set->mapRange.setPercent(percent);

    switch (set->direction)
    {
    case false:
    {
        set->barRect.setX(set->mapRange.position());
        onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
    }
    break;
    case true:
    {
        set->barRect.setY(set->mapRange.position());
        onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
    }
    break;
    }
}

void TpScroll::setPercent(double percent)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        set->range.setPercent(percent);
        percent = set->range.percent();
        set->rectRange.setPercent(percent);
        set->mapRange.setPercent(percent);
        switch (set->direction)
        {
        case false:
        {
            set->barRect.setX(set->mapRange.position());
            onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
        }
        break;
        case true:
        {
            set->barRect.setY(set->mapRange.position());
            onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
        }
        break;
        }
    }
}

int32_t TpScroll::position()
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    int32_t position = 0;

    if (set)
    {
        position = set->range.position();
    }

    return position;
}

double TpScroll::percent()
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    double percent = 0;

    if (set)
    {
        percent = set->range.percent();
    }

    return percent;
}

void TpScroll::setRect(const TpRect &rect)
{
    this->setRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void TpScroll::setRect(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        int32_t bar = TP_MIN(w, h);
        double percent = set->range.percent();

        switch (set->direction)
        {
        case false:
        {
            set->rectRange.setRange(0, w);
        }
        break;
        case true:
        {
            set->rectRange.setRange(0, h);
        }
        break;
        }

        set->rectRange.setPercent(percent);
        TpChildWidget::setRect(x, y, w, h);
        adjustBar(this, set);
    }
}

void TpScroll::setNormalBarColor(uint32_t color)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        set->normalBarColor = color;
    }
}

void TpScroll::setNormalBarColor(TpColors &color)
{
    this->setNormalBarColor(color.rgba());
}

void TpScroll::setHoverBarColor(uint32_t color)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        set->hoverBarColor = color;
    }
}

void TpScroll::setHoverBarColor(TpColors &color)
{
    this->setHoverBarColor(color.rgba());
}

void TpScroll::setDownBarColor(uint32_t color)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        set->downBarColor = color;
    }
}

void TpScroll::setDownBarColor(TpColors &color)
{
    this->setDownBarColor(color.rgba());
}

void TpScroll::setLineColor(uint32_t color)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (set)
    {
        set->lineColor = color;
    }
}

void TpScroll::setLineColor(TpColors &color)
{
    this->setLineColor(color.rgba());
}

bool TpScroll::onMouseMoveEvent(TpMouseEvent *event)
{
    bool ret = TpChildWidget::onMouseMoveEvent(event);

    if (!ret)
        return ret;

    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    if (!set)
        return ret;

    int32_t nBarPos = 0, position = 0;
    int32_t barLength = 0;

    if (event->state())
    {
        if (set->down)
        {
            switch (set->direction)
            {
            case false:
            {
                if (this->width() == set->barRect.width())
                {
                    return false;
                }

                nBarPos = event->pos().x() - set->delta;

                if (nBarPos <= 1)
                {
                    nBarPos = 1;
                }
                else if (nBarPos >= (this->width() - set->barRect.width() - 1))
                {
                    nBarPos = this->width() - set->barRect.width() - 1;
                }

                barLength = set->barRect.width();
                set->barRect.setX(nBarPos);
            }
            break;
            case true:
            {
                if (this->height() == set->barRect.height())
                {
                    return false;
                }

                nBarPos = event->pos().y() - set->delta;

                if (nBarPos <= 1)
                {
                    nBarPos = 1;
                }
                else if (nBarPos >= (this->height() - set->barRect.height() - 1))
                {
                    nBarPos = this->height() - set->barRect.height() - 1;
                }

                barLength = set->barRect.height();
                set->barRect.setY(nBarPos);
            }
            break;
            }

            if (set->lastBarPos != nBarPos)
            {
                set->mapRange.setPosition(nBarPos);
                double percent = set->mapRange.percent();
                set->rectRange.setPercent(percent);
                set->range.setPercent(percent);
                position = set->range.position();
                onScroll.emit(this, position, this->pages(), this->pageIndex(), percent);
                set->lastBarPos = nBarPos;
            }
        }
        else
        {
            TpPoint point = event->pos();
            TpRect barRRect(set->barRect);

            if (barRRect.contains(point))
            {
                set->barColor = set->hoverBarColor;
            }

            set->down = false;
        }
    }
    else
    {
        TpPoint point = event->pos();
        TpRect barRRect(set->barRect);

        if (barRRect.contains(point))
        {
            set->barColor = set->hoverBarColor;
        }
        else
        {
            set->barColor = set->normalBarColor;
        }

        set->down = false;
    }

    this->update();

    return ret;
}

bool TpScroll::onMousePressEvent(TpMouseEvent *event)
{
    TpChildWidget::onMousePressEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    set->down = true;
    set->barColor = set->downBarColor;
    TpRect barRRect(set->barRect);

    switch (set->direction)
    {
    case false:
    {
        TpPoint point(event->pos().x(), 1);

        if (barRRect.contains(point) == false)
        {
            set->rectRange.setPosition(event->pos().x());
            double percent = set->rectRange.percent();
            set->range.setPercent(percent);
            set->mapRange.setPercent(percent);
            set->barRect.setX(set->mapRange.position());
            onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
        }
        else
        {
            set->delta = event->pos().x() - set->barRect.x();
        }
    }
    break;
    case true:
    {
        TpPoint point(1, event->pos().y());

        if (barRRect.contains(point) == false)
        {
            set->rectRange.setPosition(event->pos().y());
            double percent = set->rectRange.percent();
            set->range.setPercent(percent);
            set->mapRange.setPercent(percent);
            set->barRect.setY(set->mapRange.position());
            onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
        }
        else
        {
            set->delta = event->pos().y() - set->barRect.y();
        }
    }
    break;
    }

    return true;
}

bool TpScroll::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpChildWidget::onMouseRleaseEvent(event);
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

    set->down = false;
    set->barColor = set->hoverBarColor;
    update();

    return true;
}

bool TpScroll::onFocusEvent(TpFocusEvent *event)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    bool ret = false;

    if (set)
    {
        ret = TpChildWidget::onFocusEvent(event);

        if (ret)
        {
            if (event->focused() == false)
            {
                set->down = false;
                set->barColor = set->normalBarColor;
                this->update();
            }
        }
    }

    return ret;
}

bool TpScroll::onLeaveEvent(TpLeaveEvent *event)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    bool ret = false;

    if (!set)
        return ret;

    ret = TpChildWidget::onLeaveEvent(event);

    if (!ret)
        return ret;

    if (event->leave() == false)
    {
        if (set->down == false)
        {
            set->barColor = set->normalBarColor;
            this->update();
        }
    }

    return ret;
}

bool TpScroll::onPaintEvent(TpPaintEvent *event)
{
    ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
    bool ret = false;

    if (!set)
        return ret;

    ret = TpChildWidget::onPaintEvent(event);

    if (!ret)
        return ret;

    TpPainter *canvas = event->canvas();
    uint8_t alpha = mapAlpha((uint8_t)(set->barColor & 0x000000ff), this->alpha());
    int32_t x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    switch (set->direction)
    {
    case false:
    {
        x0 = set->barRect.x();
        y0 = set->barRect.y();
        x1 = x0 + set->barRect.width() - 1;
        y1 = set->barRect.height();
    }
    break;
    case true:
    {
        x0 = set->barRect.x();
        y0 = set->barRect.y();
        x1 = set->barRect.width();
        y1 = y0 + set->barRect.height() - 1;
    }
    break;
    }

    canvas->setBrush(TpBrush(tinyPiX::NoBrush));
    canvas->setPen((set->lineColor & 0xffffff00) | alpha);
    canvas->drawRect(x0, y0, x1, y1);

    canvas->setBrush(TpBrush((set->barColor & 0xffffff00) | alpha));
    canvas->drawRect(0, 0, this->width(), this->height());

    return ret;
}
