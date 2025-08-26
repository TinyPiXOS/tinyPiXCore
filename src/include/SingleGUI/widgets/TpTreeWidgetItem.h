/***
 * @Author: hywang
 * @Date: 2024-06-14 10:32:44
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-14 16:24:31
 * @FilePath: /pix-singlegui/PixSingleGUI/include/SingleGUI/widgets/TpTreeWidgetItem.h
 * @Description:
 * @
 * @PiXOS
 */

#ifndef __TP_TREEWIDGETITEM_H
#define __TP_TREEWIDGETITEM_H

#include "TpGlobal.h"
#include "TpFont.h"
#include "TpVariant.h"
#include "TpList.h"
#include "TpString.h"
#include "TpColors.h"

TP_DEF_VOID_TYPE_VAR(IPiTreeWidgetItemPrivData);

class TpTreeWidget;
class TpTreeWidgetItem
{
public:
    TpTreeWidgetItem(const TpString &text);
    TpTreeWidgetItem(TpTreeWidgetItem *parent, const TpString &text);
    TpTreeWidgetItem(TpTreeWidget *treeview);

    TpTreeWidget *treeWidget();

    void setSelected(bool select);
    bool isSelected() const;

    void setHidden(bool hide);
    bool isHidden() const;

    void setExpanded(bool expand);
    bool isExpanded() const;

    void setFirstColumnSpanned(bool span);
    bool isFirstColumnSpanned() const;

    void setDisabled(bool disabled);
    bool isDisabled() const;

    TpString text(int32_t column);
    void setText(int32_t column, const TpString &text);

    // TpIcon icon(int32_t column) const;
    // void setIcon(int32_t column, const TpIcon &icon);

    TpString statusTip(int32_t column);
    void setStatusTip(int32_t column, const TpString &statusTip);

    TpString toolTip(int32_t column);
    void setToolTip(int32_t column, const TpString &toolTip);

    TpFont font(int32_t column);
    void setFont(int32_t column, const TpFont &font);

    tinyPiX::AlignmentFlag textAlignment(int32_t column);
    void setTextAlignment(int32_t column, tinyPiX::AlignmentFlag alignment);

    TpColors textColor(int32_t column);
    void setTextColor(int32_t column, const TpColors &color);

    tinyPiX::CheckState checkState(int32_t column);
    void setCheckState(int32_t column, tinyPiX::CheckState state);

    ItpSize sizeHint(int32_t column);
    void setSizeHint(int32_t column, const ItpSize &size);

    virtual TpVariant data(int32_t column, int32_t role) const;
    virtual void setData(int32_t column, int32_t role, const TpVariant &value);

    virtual bool operator<(const TpTreeWidgetItem &other) const;

    void setParent(TpTreeWidgetItem* parent);
    TpTreeWidgetItem *parent();
    TpTreeWidgetItem *child(int32_t index);

    int32_t childCount();
    int32_t columnCount();
    int32_t indexOfChild(TpTreeWidgetItem *child) const;

    void addChild(TpTreeWidgetItem *child);
    void insertChild(int32_t index, TpTreeWidgetItem *child);
    void removeChild(TpTreeWidgetItem *child);
    TpTreeWidgetItem *takeChild(int32_t index);

    void addChildren(const TpList<TpTreeWidgetItem *> &children);
    void insertChildren(int32_t index, const TpList<TpTreeWidgetItem *> &children);
    TpList<TpTreeWidgetItem *> takeChildren();
    void clear();

    void sortChildren(int32_t column, tinyPiX::SortOrder order);

private:
    IPiTreeWidgetItemPrivData *privData;
};

#endif