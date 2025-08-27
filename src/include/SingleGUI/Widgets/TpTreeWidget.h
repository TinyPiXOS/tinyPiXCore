/***
 * @Author: hywang
 * @Date: 2024-06-14 10:18:05
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-14 10:22:31
 * @FilePath: /pix-singlegui/PixSingleGUI/include/SingleGUI/Widgets/TpTreeWidget.h
 * @Description:
 * @
 * @PiXOS
 */

#ifndef __TP_TREEWIDGET_H
#define __TP_TREEWIDGET_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "TpTreeWidgetItem.h"
#include "TpVector.h"
#include "TpString.h"
#include "TpList.h"
#include "TpEvent.h"

TP_DEF_VOID_TYPE_VAR(IPiTreeWidgetPrivData);

/// @brief 树形基础控件，提供树图绘制显示功能
class TpTreeWidget
    : public TpChildWidget
{
public:
    TpTreeWidget(TpChildWidget *parent);

    ~TpTreeWidget();

    /// @brief 获取树列数
    /// @return 树的列数
    int32_t columnCount();

    /// @brief 设置树列数
    /// @param columns 列数值
    void setColumnCount(int32_t columns);

    /// @brief 根据索引获取指定item
    /// @param index 索引，从0开始
    /// @return 返回查找到的item指针，查找失败返回nullptr
    TpTreeWidgetItem *topLevelItem(int32_t index) const;

    /// @brief 获取顶层节点数量
    /// @return 返回节点数量
    int32_t topLevelItemCount() const;

    /// @brief 插入一个顶层接待你
    /// @param index 插入的索引，从0开始
    /// @param item 插入的item指针
    void insertTopLevelItem(int32_t index, TpTreeWidgetItem *item);
    void addTopLevelItem(TpTreeWidgetItem *item);
    TpTreeWidgetItem *takeTopLevelItem(int32_t index);
    int32_t indexOfTopLevelItem(TpTreeWidgetItem *item) const;

    void setHeaderLabels(const TpVector<TpString> &labels);
    void setHeaderLabel(const TpString &label);

    TpTreeWidgetItem *currentItem() const;
    int32_t currentColumn() const;
    void setCurrentItem(TpTreeWidgetItem *item);
    void setCurrentItem(TpTreeWidgetItem *item, int32_t column);

    TpTreeWidgetItem *itemAt(const ItpPoint &p) const;
    TpTreeWidgetItem *itemAt(int32_t x, int32_t y) const;

    int32_t sortColumn() const;
    void sortItems(int32_t column, tinyPiX::SortOrder order);

    void editItem(TpTreeWidgetItem *item, int32_t column = 0);

    TpChildWidget *itemWidget(TpTreeWidgetItem *item, int32_t column) const;
    void setItemWidget(TpTreeWidgetItem *item, int32_t column, TpChildWidget *widget);
    void removeItemWidget(TpTreeWidgetItem *item, int32_t column);

    bool isItemSelected(const TpTreeWidgetItem *item) const;
    void setItemSelected(const TpTreeWidgetItem *item, bool select);

    TpList<TpTreeWidgetItem *> selectedItems() const;
    TpList<TpTreeWidgetItem *> findItems(const TpString &text, tinyPiX::MatchFlags flags,
                                         int32_t column = 0) const;

    bool isItemExpanded(const TpTreeWidgetItem *item) const;
    void setItemExpanded(const TpTreeWidgetItem *item, bool expand);

    void setSelectionModel(tinyPiX::ItemSelectionMode selectMode);

    void expandItem(const TpTreeWidgetItem *item);
    void collapseItem(const TpTreeWidgetItem *item);
    void clear();

public
signals:
    /// @brief 节点鼠标按下事件，左键按下即触发
    /// @param TpTreeWidgetItem 点击的节点指针
    /// @param int32_t 点击的节点索引
    declare_signal(itemPressed, TpTreeWidgetItem *, int32_t);
    declare_signal(itemClicked, TpTreeWidgetItem *, int32_t);
    declare_signal(itemDoubleClicked, TpTreeWidgetItem *, int32_t);
    declare_signal(itemChanged, TpTreeWidgetItem *, int32_t);
    declare_signal(itemExpanded, TpTreeWidgetItem *);
    declare_signal(itemCollapsed, TpTreeWidgetItem *);
    // current, previous
    declare_signal(currentItemChanged, TpTreeWidgetItem *, TpTreeWidgetItem *);

protected:
    virtual bool onMoveEvent(TpObjectMoveEvent *event) override;

    virtual bool onResizeEvent(TpObjectResizeEvent *event) override;

    virtual bool onKeyPressEvent(TpKeyboardEvent *event) override;
    virtual bool onKeyReleaseEvent(TpKeyboardEvent *event) override;

    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;
    virtual bool onWheelEvent(TpWheelEvent *event) override;

    virtual bool onFocusEvent(TpObjectFocusEvent *event) override;
    virtual bool onLeaveEvent(TpObjectLeaveEvent *event) override;

    virtual bool onPaintEvent(TpObjectPaintEvent *event) override;

private:
    uint32_t getItemIndex(const uint32_t &_x, const uint32_t &_y);

    void RefreshShowIndex();

    // 查询该节点下所有子节点数量，包括二级、三级节点，直到查询最底层
    uint32_t quetyChildCount(TpTreeWidgetItem *item);

    /***
     * @description: 递归绘制item
     * @param {TpCanvas} *paint
     * @param {TpTreeWidgetItem} *item
     * @param {uint32_t} &findIndex 当前查询的index
     * @param {uint32_t} &globalCount  已绘制的item计数
     * @return {*}
     */
    void DrawItem(TpCanvas *paint, TpTreeWidgetItem *item, uint32_t &findIndex, uint32_t &globalCount);

private:
    IPiTreeWidgetPrivData *privData;
};

#endif