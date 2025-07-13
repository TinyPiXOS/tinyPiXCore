/***
 * @Author: hywang
 * @Date: 2024-06-14 10:32:44
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-14 16:24:31
 * @FilePath: /pix-singlegui/PixSingleGUI/include/SingleGUI/widgets/tpTreeWidgetItem.h
 * @Description:
 * @
 * @PiXOS
 */

#ifndef __TP_TREEWIDGETITEM_H
#define __TP_TREEWIDGETITEM_H

#include "tpGlobal.h"
#include "tpFont.h"
#include "tpVariant.h"
#include "tpList.h"
#include "tpString.h"
#include "tpColors.h"

TP_DEF_VOID_TYPE_VAR(IPiTreeWidgetItemPrivData);

class tpTreeWidget;
class tpTreeWidgetItem
{
public:
    tpTreeWidgetItem(const tpString &text);
    tpTreeWidgetItem(tpTreeWidgetItem *parent, const tpString &text);
    tpTreeWidgetItem(tpTreeWidget *treeview);

    tpTreeWidget *treeWidget();

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

    tpString text(int32_t column);
    void setText(int32_t column, const tpString &text);

    // tpIcon icon(int32_t column) const;
    // void setIcon(int32_t column, const tpIcon &icon);

    tpString statusTip(int32_t column);
    void setStatusTip(int32_t column, const tpString &statusTip);

    tpString toolTip(int32_t column);
    void setToolTip(int32_t column, const tpString &toolTip);

    tpFont font(int32_t column);
    void setFont(int32_t column, const tpFont &font);

    tinyPiX::AlignmentFlag textAlignment(int32_t column);
    void setTextAlignment(int32_t column, tinyPiX::AlignmentFlag alignment);

    tpColors textColor(int32_t column);
    void setTextColor(int32_t column, const tpColors &color);

    tinyPiX::CheckState checkState(int32_t column);
    void setCheckState(int32_t column, tinyPiX::CheckState state);

    ItpSize sizeHint(int32_t column);
    void setSizeHint(int32_t column, const ItpSize &size);

    virtual tpVariant data(int32_t column, int32_t role) const;
    virtual void setData(int32_t column, int32_t role, const tpVariant &value);

    virtual bool operator<(const tpTreeWidgetItem &other) const;

    void setParent(tpTreeWidgetItem* parent);
    tpTreeWidgetItem *parent();
    tpTreeWidgetItem *child(int32_t index);

    int32_t childCount();
    int32_t columnCount();
    int32_t indexOfChild(tpTreeWidgetItem *child) const;

    void addChild(tpTreeWidgetItem *child);
    void insertChild(int32_t index, tpTreeWidgetItem *child);
    void removeChild(tpTreeWidgetItem *child);
    tpTreeWidgetItem *takeChild(int32_t index);

    void addChildren(const tpList<tpTreeWidgetItem *> &children);
    void insertChildren(int32_t index, const tpList<tpTreeWidgetItem *> &children);
    tpList<tpTreeWidgetItem *> takeChildren();
    void clear();

    void sortChildren(int32_t column, tinyPiX::SortOrder order);

private:
    IPiTreeWidgetItemPrivData *privData;
};

#endif