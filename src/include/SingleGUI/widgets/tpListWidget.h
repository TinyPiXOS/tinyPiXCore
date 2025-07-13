/***
 * @Author: hywang
 * @Date: 2024-05-31 09:39:28
 * @LastEditors: hywang
 * @LastEditTime: 2024-05-31 09:58:23
 * @FilePath: /pix-singlegui/PixSingleGUI/include/SingleGUI/widgets/tpListWidget.h
 * @Description: ListWidget实现类
 * @
 * @PiXOS
 */

#ifndef __TP_LISTWIDGET_H
#define __TP_LISTWIDGET_H

#include "tpListWidgetItem.h"
#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpList.h"
#include "tpGlobal.h"
#include "tpEvent.h"

TP_DEF_VOID_TYPE_VAR(IPiListWidgetPrivData);

class tpListWidget : public tpChildWidget
{
public:
    tpListWidget(tpChildWidget *parent);

    ~tpListWidget();

    tpListWidgetItem *item(int row) const;
    int row(const tpListWidgetItem *item) const;
    void insertItem(int row, tpListWidgetItem *item);
    void insertItem(int row, const tpString &label);
    void insertItems(int row, const tpList<tpString> &labels);
    void addItem(const tpString &label) { insertItem(count(), label); }
    void addItem(tpListWidgetItem *item);
    void addItems(const tpList<tpString> &labels) { insertItems(count(), labels); }
    tpListWidgetItem *takeItem(int row);
    int count() const;

    tpListWidgetItem *currentItem() const;
    void setCurrentItem(tpListWidgetItem *item);

    int currentRow() const;
    void setCurrentRow(int row);

    tpListWidgetItem *itemAt(const ItpPoint &p);
    tpListWidgetItem *itemAt(int x, int y);

    void sortItems(tinyPiX::SortOrder order = tinyPiX::AscendingOrder);
    void setSortingEnabled(bool enable);
    bool isSortingEnabled() const;

    tpChildWidget *itemWidget(tpListWidgetItem *item) const;
    void setItemWidget(tpListWidgetItem *item, tpChildWidget *widget);
    void removeItemWidget(tpListWidgetItem *item);

    bool isItemSelected(const tpListWidgetItem *item) const;
    void setItemSelected(const tpListWidgetItem *item, bool select);

    tpList<tpListWidgetItem *> selectedItems() const;
    tpList<tpListWidgetItem *> findItems(const tpString &text, tinyPiX::MatchFlags flags) const;

    // bool isItemHidden(const tpListWidgetItem *item) const;
    // void setItemHidden(const tpListWidgetItem *item, bool hide);

    // void dropEvent(tpDropEvent *event) override;

    void clear();

    ItpSize sizeHint() const;
    void setSizeHint(const ItpSize &size);

    void setSelectionModel(tinyPiX::ItemSelectionMode selectMode);

public
signals:
    declare_signal(itemPressed, tpListWidgetItem *);
    declare_signal(itemClicked, tpListWidgetItem *);
    // declare_signal(itemDoubleClicked, tpListWidgetItem *);

    // current, previous
    declare_signal(currentItemChanged, tpListWidgetItem *, tpListWidgetItem *);
    declare_signal(currentTextChanged, tpString);
    declare_signal(currentRowChanged, uint32_t); // 行号，从0开始

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

private:
    IPiListWidgetPrivData *privData;
};

#endif