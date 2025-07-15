#include "tpTabWidget.h"
#include "tpVector.h"
#include "tpTabBar.h"

struct tpTabWidgetData
{
    // 没有用布局管理tabbar，因为考虑未来调整bar在上下左右四个方向，用布局后期修改麻烦
    tpTabBar *tabBar;
    tpVector<tpChildWidget *> tabWidgetList;

    tpTabWidgetData()
    {
    }
    ~tpTabWidgetData()
    {
        tabWidgetList.clear();
        tabBar->setParent(nullptr);
        delete tabBar;
        tabBar = nullptr;
    }
};

tpTabWidget::tpTabWidget(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpTabWidgetData *widgetData = new tpTabWidgetData();

    widgetData->tabBar = new tpTabBar(this);
    setEnableBackGroundColor(false);

    data_ = widgetData;

    // 按钮切换调整显示窗口
    connect(widgetData->tabBar, indexChanged, this, &tpTabWidget::slotTabBarIndexChanged);
}

tpTabWidget::~tpTabWidget()
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);
    if (widgetData)
    {
        delete widgetData;
        widgetData = nullptr;
    }
}

int32_t tpTabWidget::addTab(tpChildWidget *widget, const tpString &text)
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);

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

int32_t tpTabWidget::insertTab(int32_t index, tpChildWidget *widget, const tpString &text)
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);

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

void tpTabWidget::removeTab(int32_t index)
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);
    widgetData->tabBar->removeButton(index);
    widgetData->tabWidgetList.remove(index);
}

tpString tpTabWidget::tabText(int32_t index) const
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);

    return widgetData->tabBar->tabText(index);
}

void tpTabWidget::setTabText(int32_t index, const tpString &text)
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);

    widgetData->tabBar->setTabText(index, text);
}

int32_t tpTabWidget::currentIndex() const
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);

    return widgetData->tabBar->currendIndex();
}

tpChildWidget *tpTabWidget::currentWidget() const
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);

    uint32_t curIndex = widgetData->tabBar->currendIndex();
    if (curIndex >= widgetData->tabWidgetList.size())
        return nullptr;

    return widgetData->tabWidgetList.at(curIndex);
}

tpChildWidget *tpTabWidget::widget(int32_t index) const
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);
    if (index >= widgetData->tabWidgetList.size())
        return nullptr;

    return widgetData->tabWidgetList.at(index);
}

int32_t tpTabWidget::indexOf(tpChildWidget *widget) const
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);
    for (int i = 0; i < widgetData->tabWidgetList.size(); ++i)
    {
        tpChildWidget *curWidget = widgetData->tabWidgetList.at(i);
        if (curWidget == widget)
            return i;
    }

    return 0;
}

int32_t tpTabWidget::count() const
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);

    return widgetData->tabWidgetList.size();
}

void tpTabWidget::clear()
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);
    widgetData->tabWidgetList.clear();
    widgetData->tabBar->clear();
}

tpTabBar *tpTabWidget::tabBar() const
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);

    return widgetData->tabBar;
}

void tpTabWidget::setCurrentIndex(int32_t index)
{
}

void tpTabWidget::setCurrentWidget(tpChildWidget *widget)
{
}

bool tpTabWidget::onResizeEvent(tpObjectResizeEvent *event)
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);

    widgetData->tabBar->setSize(width(), widgetData->tabBar->height());
    widgetData->tabBar->move(0, height() - widgetData->tabBar->height());

    slotTabBarIndexChanged(widgetData->tabBar->currendIndex());

    return true;
}

bool tpTabWidget::onMoveEvent(tpObjectMoveEvent *event)
{
    return true;
}

bool tpTabWidget::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    return true;
}

bool tpTabWidget::eventFilter(tpObject *watched, tpEvent *event)
{
    return false;
}

void tpTabWidget::slotTabBarIndexChanged(uint32_t index)
{
    tpTabWidgetData *widgetData = static_cast<tpTabWidgetData *>(data_);

    if (index >= widgetData->tabWidgetList.size())
        return;

    for (int i = 0; i < widgetData->tabWidgetList.size(); ++i)
    {
        tpChildWidget *curWidget = widgetData->tabWidgetList.at(i);
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
