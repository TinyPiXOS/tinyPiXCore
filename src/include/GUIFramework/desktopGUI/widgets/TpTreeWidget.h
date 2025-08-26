#ifndef __DESKTOP_GUI_TP_TREE_WIDGET_H
#define __DESKTOP_GUI_TP_TREE_WIDGET_H

#include "TpChildWidget.h"
#include "TpEvent.h"
#include "TpSignalSlot.h"
#include "GUIFramework/desktopGUI/FrameworkGlobal.h"
#include "GUIFramework/desktopGUI/widgets/TpTreeWidgetItem.h"

DESKTOP_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ItpTreeWidgetData);

class TpTreeWidget
    : public TpChildWidget
{
public:
    TpTreeWidget(TpChildWidget *parent);
    ~TpTreeWidget();

    /// @brief 设置标题栏是否显示，默认显示
    /// @param visible true显示
    void setTitleVisible(const bool &visible = true);

    /// @brief 设置树标题
    /// @param title 标题
    void setTitle(const TpString &title);

    /// @brief 设置树标题icon
    /// @param titleIconPath 图标路径
    void setTitleIcon(const TpString &titleIconPath);

    /// @brief 添加顶层节点
    /// @param text  节点文本
    TpTreeWidgetItem *addTopItem(const TpString &text);

    /// @brief 添加顶层节点
    /// @param topItem 节点Item
    void addTopItem(TpTreeWidgetItem *topItem);

    /// @brief 插入顶层节点
    /// @param index 插入的下标，从0开始
    /// @param item 插入的顶层item
    void insertItem(int32_t index, TpTreeWidgetItem *item);

    /// @brief 顶层节点计数
    /// @return 顶层节点数量
    int32_t topLevelItemCount() const;

    /// @brief 根据索引获取顶层节点
    /// @param index 索引，0开始
    /// @return 返回指定索引item，不存在返回nullptr
    TpTreeWidgetItem *topLevelItem(int32_t index) const;

public
signals:
    /// @brief item被鼠标按下事件
    /// @param 点击item
    declare_signal(itemPressed, TpTreeWidgetItem *);

    /// @brief item展开事件
    /// @param item
    declare_signal(itemExpanded, TpTreeWidgetItem *);

    /// @brief item收起事件
    /// @param item
    declare_signal(itemCollapsed, TpTreeWidgetItem *);

    /// @brief item切换事件  <previous , current>
    /// @param item
    declare_signal(currentItemChanged, TpTreeWidgetItem *, TpTreeWidgetItem *);

protected:
    virtual bool onPaintEvent(TpObjectPaintEvent *event) override;

    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onWheelEvent(TpWheelEvent *event) override;

    virtual bool onLeaveEvent(TpObjectLeaveEvent *event) override;

    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;

    virtual bool onResizeEvent(TpObjectResizeEvent *event) override;

private:
    void drawItem(TpObjectPaintEvent *event, TpTreeWidgetItem *topItem, const uint32_t &drawItemStartY, uint32_t &curIndex);

    // 获取item是第几级节点
    uint32_t itemParentCount(TpTreeWidgetItem *item);

    int32_t queryPointIndex(const ItpPoint &point);

private:
    ItpTreeWidgetData *data_;
};

DESKTOP_GUI_NAMESPACE_END

#endif
