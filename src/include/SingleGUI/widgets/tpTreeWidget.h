/***
 * @Author: hywang
 * @Date: 2024-06-14 10:18:05
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-14 10:22:31
 * @FilePath: /pix-singlegui/PixSingleGUI/include/SingleGUI/widgets/tpTreeWidget.h
 * @Description:
 * @
 * @PiXOS
 */

#ifndef __TP_TREEWIDGET_H
#define __TP_TREEWIDGET_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpTreeWidgetItem.h"
#include "tpVector.h"
#include "tpString.h"
#include "tpList.h"
#include "tpEvent.h"

TP_DEF_VOID_TYPE_VAR(IPiTreeWidgetPrivData);

/// @brief 树形基础控件，提供树图绘制显示功能
class tpTreeWidget
    : public tpChildWidget
{
public:
    tpTreeWidget(tpChildWidget *parent);

    ~tpTreeWidget();

    /// @brief 获取树列数
    /// @return 树的列数
    int32_t columnCount();

    /// @brief 设置树列数
    /// @param columns 列数值
    void setColumnCount(int32_t columns);

    /// @brief 根据索引获取指定item
    /// @param index 索引，从0开始
    /// @return 返回查找到的item指针，查找失败返回nullptr
    tpTreeWidgetItem *topLevelItem(int32_t index) const;

    /// @brief 获取顶层节点数量
    /// @return 返回节点数量
    int32_t topLevelItemCount() const;

    /// @brief 插入一个顶层接待你
    /// @param index 插入的索引，从0开始
    /// @param item 插入的item指针
    void insertTopLevelItem(int32_t index, tpTreeWidgetItem *item);
    void addTopLevelItem(tpTreeWidgetItem *item);
    tpTreeWidgetItem *takeTopLevelItem(int32_t index);
    int32_t indexOfTopLevelItem(tpTreeWidgetItem *item) const;

    void setHeaderLabels(const tpVector<tpString> &labels);
    void setHeaderLabel(const tpString &label);

    tpTreeWidgetItem *currentItem() const;
    int32_t currentColumn() const;
    void setCurrentItem(tpTreeWidgetItem *item);
    void setCurrentItem(tpTreeWidgetItem *item, int32_t column);

    tpTreeWidgetItem *itemAt(const ItpPoint &p) const;
    tpTreeWidgetItem *itemAt(int32_t x, int32_t y) const;

    int32_t sortColumn() const;
    void sortItems(int32_t column, tinyPiX::SortOrder order);

    void editItem(tpTreeWidgetItem *item, int32_t column = 0);

    tpChildWidget *itemWidget(tpTreeWidgetItem *item, int32_t column) const;
    void setItemWidget(tpTreeWidgetItem *item, int32_t column, tpChildWidget *widget);
    void removeItemWidget(tpTreeWidgetItem *item, int32_t column);

    bool isItemSelected(const tpTreeWidgetItem *item) const;
    void setItemSelected(const tpTreeWidgetItem *item, bool select);

    tpList<tpTreeWidgetItem *> selectedItems() const;
    tpList<tpTreeWidgetItem *> findItems(const tpString &text, tinyPiX::MatchFlags flags,
                                         int32_t column = 0) const;

    bool isItemExpanded(const tpTreeWidgetItem *item) const;
    void setItemExpanded(const tpTreeWidgetItem *item, bool expand);

    void setSelectionModel(tinyPiX::ItemSelectionMode selectMode);

    void expandItem(const tpTreeWidgetItem *item);
    void collapseItem(const tpTreeWidgetItem *item);
    void clear();

public
signals:
    /// @brief 节点鼠标按下事件，左键按下即触发
    /// @param tpTreeWidgetItem 点击的节点指针
    /// @param int32_t 点击的节点索引
    declare_signal(itemPressed, tpTreeWidgetItem *, int32_t);
    declare_signal(itemClicked, tpTreeWidgetItem *, int32_t);
    declare_signal(itemDoubleClicked, tpTreeWidgetItem *, int32_t);
    declare_signal(itemChanged, tpTreeWidgetItem *, int32_t);
    declare_signal(itemExpanded, tpTreeWidgetItem *);
    declare_signal(itemCollapsed, tpTreeWidgetItem *);
    // current, previous
    declare_signal(currentItemChanged, tpTreeWidgetItem *, tpTreeWidgetItem *);

protected:
    virtual bool onMoveEvent(tpObjectMoveEvent *event) override;

    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual bool onKeyPressEvent(tpKeyboardEvent *event) override;
    virtual bool onKeyReleaseEvent(tpKeyboardEvent *event) override;

    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(tpMouseEvent *event) override;
    virtual bool onWheelEvent(tpWheelEvent *event) override;

    virtual bool onFocusEvent(tpObjectFocusEvent *event) override;
    virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;

    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

private:
    uint32_t getItemIndex(const uint32_t &_x, const uint32_t &_y);

    void RefreshShowIndex();

    // 查询该节点下所有子节点数量，包括二级、三级节点，直到查询最底层
    uint32_t quetyChildCount(tpTreeWidgetItem *item);

    /***
     * @description: 递归绘制item
     * @param {tpCanvas} *paint
     * @param {tpTreeWidgetItem} *item
     * @param {uint32_t} &findIndex 当前查询的index
     * @param {uint32_t} &globalCount  已绘制的item计数
     * @return {*}
     */
    void DrawItem(tpCanvas *paint, tpTreeWidgetItem *item, uint32_t &findIndex, uint32_t &globalCount);

private:
    IPiTreeWidgetPrivData *privData;
};

#endif