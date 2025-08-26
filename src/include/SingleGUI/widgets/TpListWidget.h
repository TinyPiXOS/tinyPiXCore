/***
 * @Author: hywang
 * @Date: 2024-05-31 09:39:28
 * @LastEditors: hywang
 * @LastEditTime: 2024-05-31 09:58:23
 * @FilePath: /pix-singlegui/PixSingleGUI/include/SingleGUI/widgets/TpListWidget.h
 * @Description: ListWidget实现类
 * @
 * @PiXOS
 */

#ifndef __TP_LISTWIDGET_H
#define __TP_LISTWIDGET_H

#include "TpListWidgetItem.h"
#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "TpList.h"
#include "TpGlobal.h"
#include "TpEvent.h"

TP_DEF_VOID_TYPE_VAR(IPiListWidgetPrivData);

class TpListWidget : public TpChildWidget
{
public:
    TpListWidget(TpChildWidget *parent);

    ~TpListWidget();

    TpListWidgetItem *item(int row) const;
    int row(const TpListWidgetItem *item) const;
    void insertItem(int row, TpListWidgetItem *item);
    void insertItem(int row, const TpString &label);
    void insertItems(int row, const TpList<TpString> &labels);
    void addItem(const TpString &label) { insertItem(count(), label); }
    void addItem(TpListWidgetItem *item);
    void addItems(const TpList<TpString> &labels) { insertItems(count(), labels); }
    TpListWidgetItem *takeItem(int row);
    int count() const;

    TpListWidgetItem *currentItem() const;
    void setCurrentItem(TpListWidgetItem *item);

    int currentRow() const;
    void setCurrentRow(int row);

    TpListWidgetItem *itemAt(const ItpPoint &p);
    TpListWidgetItem *itemAt(int x, int y);

    void sortItems(tinyPiX::SortOrder order = tinyPiX::AscendingOrder);
    void setSortingEnabled(bool enable);
    bool isSortingEnabled() const;

    TpChildWidget *itemWidget(TpListWidgetItem *item) const;
    void setItemWidget(TpListWidgetItem *item, TpChildWidget *widget);
    void removeItemWidget(TpListWidgetItem *item);

    bool isItemSelected(const TpListWidgetItem *item) const;
    void setItemSelected(const TpListWidgetItem *item, bool select);

    TpList<TpListWidgetItem *> selectedItems() const;
    TpList<TpListWidgetItem *> findItems(const TpString &text, tinyPiX::MatchFlags flags) const;

    // bool isItemHidden(const TpListWidgetItem *item) const;
    // void setItemHidden(const TpListWidgetItem *item, bool hide);

    // void dropEvent(TpDropEvent *event) override;

    void clear();

    ItpSize sizeHint() const;
    void setSizeHint(const ItpSize &size);

    void setSelectionModel(tinyPiX::ItemSelectionMode selectMode);

public
signals:
    declare_signal(itemPressed, TpListWidgetItem *);
    declare_signal(itemClicked, TpListWidgetItem *);
    // declare_signal(itemDoubleClicked, TpListWidgetItem *);

    // current, previous
    declare_signal(currentItemChanged, TpListWidgetItem *, TpListWidgetItem *);
    declare_signal(currentTextChanged, TpString);
    declare_signal(currentRowChanged, uint32_t); // 行号，从0开始

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

private:
    IPiListWidgetPrivData *privData;
};

#endif