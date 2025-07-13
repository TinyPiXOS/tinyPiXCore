#ifndef __DESKTOP_GUI_TP_TREE_WIDGET_ITEM_H
#define __DESKTOP_GUI_TP_TREE_WIDGET_ITEM_H

#include "tpChildWidget.h"
#include "tpEvent.h"
#include "tpSignalSlot.h"
#include "tpVariant.h"
#include "tpList.h"
#include "tpFont.h"

#include "GUIFramework/desktopGUI/FrameworkGlobal.h"
#include "GUIFramework/desktopGUI/widgets/tpTreeWidgetItem.h"

DESKTOP_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ItpTreeWidgetItemData);

class tpTreeWidget;
class tpTreeWidgetItem
    : public tpChildWidget
{
    friend class tpTreeWidget;
    
public:
    tpTreeWidgetItem(tpTreeWidgetItem *parent = nullptr);
    tpTreeWidgetItem(const tpString &text, tpTreeWidgetItem *parent = nullptr);
    tpTreeWidgetItem(const tpString &text, const tpString &iconPath, tpTreeWidgetItem *parent = nullptr);

    ~tpTreeWidgetItem();

    tpFont* font();

    void setText(const tpString &text);
    tpString text();

    void setIcon(const tpString& iconPath);
    tpShared<tpSurface> icon();

    void setExpanded(bool expand);
    bool isExpanded() const;

    void setSelected(bool selected);
    bool isSelected();

    tpVariant data(int32_t role) const;
    void setData(int32_t role, const tpVariant &value);

    void setParent(tpTreeWidgetItem *parent);
    tpTreeWidgetItem *parent();
    tpTreeWidgetItem *child(uint32_t index);

    uint32_t childCount();
    uint32_t indexOfChild(tpTreeWidgetItem *child) const;

    void addChild(tpTreeWidgetItem *child);
    void insertChild(uint32_t index, tpTreeWidgetItem *child);
    void removeChild(tpTreeWidgetItem *child);
    tpTreeWidgetItem *takeChild(uint32_t index);

    void addChildren(const tpList<tpTreeWidgetItem *> &children);
    void insertChildren(uint32_t index, const tpList<tpTreeWidgetItem *> &children);
    tpList<tpTreeWidgetItem *> takeChildren();

protected:
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

private:
    ItpTreeWidgetItemData *data_;
};

DESKTOP_GUI_NAMESPACE_END

#endif