#ifndef __DESKTOP_GUI_TP_TREE_WIDGET_ITEM_H
#define __DESKTOP_GUI_TP_TREE_WIDGET_ITEM_H

#include "TpChildWidget.h"
#include "TpEvent.h"
#include "TpSignalSlot.h"
#include "TpVariant.h"
#include "TpList.h"
#include "TpFont.h"

#include "GUIFramework/desktopGUI/FrameworkGlobal.h"
#include "GUIFramework/desktopGUI/widgets/TpTreeWidgetItem.h"

DESKTOP_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ItpTreeWidgetItemData);

class TpTreeWidget;
class TpTreeWidgetItem
    : public TpChildWidget
{
    friend class TpTreeWidget;
    
public:
    TpTreeWidgetItem(TpTreeWidgetItem *parent = nullptr);
    TpTreeWidgetItem(const TpString &text, TpTreeWidgetItem *parent = nullptr);
    TpTreeWidgetItem(const TpString &text, const TpString &iconPath, TpTreeWidgetItem *parent = nullptr);

    ~TpTreeWidgetItem();

    TpFont* font();

    void setText(const TpString &text);
    TpString text();

    void setIcon(const TpString& iconPath);
    TpImage icon();

    void setExpanded(bool expand);
    bool isExpanded() const;

    void setSelected(bool selected);
    bool isSelected();

    TpVariant data(int32_t role) const;
    void setData(int32_t role, const TpVariant &value);

    void setParent(TpTreeWidgetItem *parent);
    TpTreeWidgetItem *parent();
    TpTreeWidgetItem *child(uint32_t index);

    uint32_t childCount();
    uint32_t indexOfChild(TpTreeWidgetItem *child) const;

    void addChild(TpTreeWidgetItem *child);
    void insertChild(uint32_t index, TpTreeWidgetItem *child);
    void removeChild(TpTreeWidgetItem *child);
    TpTreeWidgetItem *takeChild(uint32_t index);

    void addChildren(const TpList<TpTreeWidgetItem *> &children);
    void insertChildren(uint32_t index, const TpList<TpTreeWidgetItem *> &children);
    TpList<TpTreeWidgetItem *> takeChildren();

protected:
    virtual bool onPaintEvent(TpObjectPaintEvent *event) override;

private:
    ItpTreeWidgetItemData *data_;
};

DESKTOP_GUI_NAMESPACE_END

#endif