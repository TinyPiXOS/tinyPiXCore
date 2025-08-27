#include "TpTabWidget.h"
#include "TpVector.h"
#include "TpTabBar.h"

struct TpTabWidgetData
{
    // 没有用布局管理tabbar，因为考虑未来调整bar在上下左右四个方向，用布局后期修改麻烦
    TpTabBar *tabBar;
    TpVector<TpChildWidget *> tabWidgetList;

    TpTabWidgetData()
    {
    }
    ~TpTabWidgetData()
    {
        tabWidgetList.clear();
        tabBar->setParent(nullptr);
        delete tabBar;
        tabBar = nullptr;
    }
};

TpTabWidget::TpTabWidget(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    TpTabWidgetData *widgetData = new TpTabWidgetData();

    widgetData->tabBar = new TpTabBar(this);
    setEnableBackGroundColor(false);

    data_ = widgetData;

    // 按钮切换调整显示窗口
    connect(widgetData->tabBar, indexChanged, this, &TpTabWidget::slotTabBarIndexChanged);
}

TpTabWidget::~TpTabWidget()
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);
    if (widgetData)
    {
        delete widgetData;
        widgetData = nullptr;
    }
}

int32_t TpTabWidget::addTab(TpChildWidget *widget, const TpString &text)
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);

    // 添加的窗体父指针应该是自己
    if (widget)
    {
        widget->setParent(this);

        if (widgetData->tabWidgetList.size() == 0)
            widget->setVisible(true);
        else
            widget->setVisible(false);
    }

    widgetData->tabWidgetList.emplace_back(widget);
    widgetData->tabBar->addButton(text);

    return widgetData->tabWidgetList.size();
}

int32_t TpTabWidget::insertTab(int32_t index, TpChildWidget *widget, const TpString &text)
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);

    if (widget)
    {
        widget->setParent(this);

        if (widgetData->tabWidgetList.size() == 0)
            widget->setVisible(true);
        else
            widget->setVisible(false);
    }

    widgetData->tabWidgetList.insertData(index, widget);
    widgetData->tabBar->insertButton(index, text);

    return index;
}

void TpTabWidget::removeTab(int32_t index)
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);
    widgetData->tabBar->removeButton(index);
    widgetData->tabWidgetList.remove(index);
}

TpString TpTabWidget::tabText(int32_t index) const
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);

    return widgetData->tabBar->tabText(index);
}

void TpTabWidget::setTabText(int32_t index, const TpString &text)
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);

    widgetData->tabBar->setTabText(index, text);
}

int32_t TpTabWidget::currentIndex() const
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);

    return widgetData->tabBar->currendIndex();
}

TpChildWidget *TpTabWidget::currentWidget() const
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);

    uint32_t curIndex = widgetData->tabBar->currendIndex();
    if (curIndex >= widgetData->tabWidgetList.size())
        return nullptr;

    return widgetData->tabWidgetList.at(curIndex);
}

TpChildWidget *TpTabWidget::widget(int32_t index) const
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);
    if (index >= widgetData->tabWidgetList.size())
        return nullptr;

    return widgetData->tabWidgetList.at(index);
}

int32_t TpTabWidget::indexOf(TpChildWidget *widget) const
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);
    for (int i = 0; i < widgetData->tabWidgetList.size(); ++i)
    {
        TpChildWidget *curWidget = widgetData->tabWidgetList.at(i);
        if (curWidget == widget)
            return i;
    }

    return 0;
}

int32_t TpTabWidget::count() const
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);

    return widgetData->tabWidgetList.size();
}

void TpTabWidget::clear()
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);
    widgetData->tabWidgetList.clear();
    widgetData->tabBar->clear();
}

TpTabBar *TpTabWidget::tabBar() const
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);

    return widgetData->tabBar;
}

void TpTabWidget::setCurrentIndex(int32_t index)
{
}

void TpTabWidget::setCurrentWidget(TpChildWidget *widget)
{
}

bool TpTabWidget::onResizeEvent(TpObjectResizeEvent *event)
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);

    widgetData->tabBar->setSize(width(), widgetData->tabBar->height());
    widgetData->tabBar->move(0, height() - widgetData->tabBar->height());

    slotTabBarIndexChanged(widgetData->tabBar->currendIndex());

    return true;
}

bool TpTabWidget::onMoveEvent(TpObjectMoveEvent *event)
{
    return true;
}

bool TpTabWidget::onPaintEvent(TpObjectPaintEvent *event)
{
    TpChildWidget::onPaintEvent(event);

    return true;
}

bool TpTabWidget::eventFilter(TpObject *watched, TpEvent *event)
{
    return false;
}

void TpTabWidget::slotTabBarIndexChanged(uint32_t index)
{
    TpTabWidgetData *widgetData = static_cast<TpTabWidgetData *>(data_);

    if (index >= widgetData->tabWidgetList.size())
        return;

    for (int i = 0; i < widgetData->tabWidgetList.size(); ++i)
    {
        TpChildWidget *curWidget = widgetData->tabWidgetList.at(i);
        if (i == index)
        {
            if (curWidget)
            {
                curWidget->setVisible(true);
                curWidget->setRect(0, 0, width(), height() - widgetData->tabBar->height());
                curWidget->update();
            }
        }
        else
        {
            if (curWidget)
            {
                curWidget->setVisible(false);
                curWidget->update();
            }
        }
    }
}
