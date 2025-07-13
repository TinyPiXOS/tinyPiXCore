#ifndef __DESKTOP_GUI_TP_TREE_WIDGET_H
#define __DESKTOP_GUI_TP_TREE_WIDGET_H

#include "tpChildWidget.h"
#include "tpEvent.h"
#include "tpSignalSlot.h"
#include "GUIFramework/desktopGUI/FrameworkGlobal.h"
#include "GUIFramework/desktopGUI/widgets/tpTreeWidgetItem.h"

DESKTOP_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ItpTreeWidgetData);

class tpTreeWidget
    : public tpChildWidget
{
public:
    tpTreeWidget(tpChildWidget *parent);
    ~tpTreeWidget();

    /// @brief 设置标题栏是否显示，默认显示
    /// @param visible true显示
    void setTitleVisible(const bool &visible = true);

    /// @brief 设置树标题
    /// @param title 标题
    void setTitle(const tpString &title);

    /// @brief 设置树标题icon
    /// @param titleIconPath 图标路径
    void setTitleIcon(const tpString &titleIconPath);

    /// @brief 添加顶层节点
    /// @param text  节点文本
    tpTreeWidgetItem *addTopItem(const tpString &text);

    /// @brief 添加顶层节点
    /// @param topItem 节点Item
    void addTopItem(tpTreeWidgetItem *topItem);

    /// @brief 插入顶层节点
    /// @param index 插入的下标，从0开始
    /// @param item 插入的顶层item
    void insertItem(int32_t index, tpTreeWidgetItem *item);

    /// @brief 顶层节点计数
    /// @return 顶层节点数量
    int32_t topLevelItemCount() const;

    /// @brief 根据索引获取顶层节点
    /// @param index 索引，0开始
    /// @return 返回指定索引item，不存在返回nullptr
    tpTreeWidgetItem *topLevelItem(int32_t index) const;

public
signals:
    /// @brief item被鼠标按下事件
    /// @param 点击item
    declare_signal(itemPressed, tpTreeWidgetItem *);

    /// @brief item展开事件
    /// @param item
    declare_signal(itemExpanded, tpTreeWidgetItem *);

    /// @brief item收起事件
    /// @param item
    declare_signal(itemCollapsed, tpTreeWidgetItem *);

    /// @brief item切换事件  <previous , current>
    /// @param item
    declare_signal(currentItemChanged, tpTreeWidgetItem *, tpTreeWidgetItem *);

protected:
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onWheelEvent(tpWheelEvent *event) override;

    virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;

    virtual bool onMouseMoveEvent(tpMouseEvent *event) override;

    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

private:
    void drawItem(tpObjectPaintEvent *event, tpTreeWidgetItem *topItem, const uint32_t &drawItemStartY, uint32_t &curIndex);

    // 获取item是第几级节点
    uint32_t itemParentCount(tpTreeWidgetItem *item);

    int32_t queryPointIndex(const ItpPoint &point);

private:
    ItpTreeWidgetData *data_;
};

DESKTOP_GUI_NAMESPACE_END

#endif
