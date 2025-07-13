#ifndef __TP_TAB_WIDGET_H
#define __TP_TAB_WIDGET_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include <tpString.h>

TP_DEF_VOID_TYPE_VAR(ItpTabWidgetData);
class tpTabBar;
/// @brief 菜单在底部的桌面tab
class tpTabWidget : public tpChildWidget
{
public:
    tpTabWidget(tpChildWidget *parent = nullptr);

    virtual ~tpTabWidget();

    /// @brief 添加一个tab页
    /// @param widget 该页对应的窗口
    /// @param text tab文本
    /// @return tab索引
    int32_t addTab(tpChildWidget *widget, const tpString &text);

    /// @brief 插入一个tab页
    /// @param index 插入的索引，从0开始
    /// @param widget 该页对应的窗口
    /// @param text tab文本
    /// @return tab索引
    int32_t insertTab(int32_t index, tpChildWidget *widget, const tpString &text);

    /// @brief 指定索引删除指定tab，tab对应的外部窗体不会释放
    /// @param index tab索引
    void removeTab(int32_t index);

    /// @brief 指定索引获取tab标题文本
    /// @param index 索引
    /// @return 文本
    tpString tabText(int32_t index) const;
    /// @brief 指定索引设置tab标题文本
    /// @param index 索引值
    /// @param text 标题文本
    void setTabText(int32_t index, const tpString &text);

    /// @brief 获取当前选中索引
    /// @return 索引值
    int32_t currentIndex() const;
    /// @brief 获取当前选中的索引对应的widget，无则返回nullptr
    /// @return 窗体指针
    tpChildWidget *currentWidget() const;
    /// @brief 指定索引获取对应的窗体指针，无则返回nullptr
    /// @param index 索引值
    /// @return 窗体指针
    tpChildWidget *widget(int32_t index) const;
    /// @brief 指定窗体指针获取对应的索引值
    /// @param widget 窗体指针
    /// @return 查询到的索引，查询失败返回-1
    int32_t indexOf(tpChildWidget *widget) const;
    /// @brief 获取总数量
    /// @return 总数量
    int32_t count() const;

    /// @brief 清除所有tab，所有外部给入的指针不会释放
    void clear();

    /// @brief 获取tab窗口的bar
    /// @return tabbar指针
    tpTabBar *tabBar() const;

    /// @brief 设置当前选中索引
    /// @param index 索引值
    void setCurrentIndex(int32_t index);
    /// @brief 设置当前选中widget
    /// @param widget 窗体指针
    void setCurrentWidget(tpChildWidget *widget);

public:
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;
    virtual bool onMoveEvent(tpObjectMoveEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpTabWidget); }

private:
    void slotTabBarIndexChanged(uint32_t index);

public
signals:
    declare_signal(indexChanged, int32_t);

private:
    ItpTabWidgetData *data_;
};

#endif
