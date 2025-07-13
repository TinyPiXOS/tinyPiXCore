/*** 
 * @Author: hywang
 * @Date: 2024-05-31 09:54:54
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-17 10:59:49
 * @FilePath: /pix-singlegui/PixSingleGUI/include/SingleGUI/widgets/tpListWidgetItem.h
 * @Description: 
 * @
 * @PiXOS
 */

/***
 * @Author: hywang
 * @Date: 2024-05-31 09:54:54
 * @LastEditors: hywang
 * @LastEditTime: 2024-05-31 10:00:47
 * @FilePath: /pix-singlegui/PixSingleGUI/include/SingleGUI/widgets/tpListWidgetItem.h
 * @Description: ListWidget的单个条目
 * @
 * @PiXOS
 */

#ifndef __TP_LISTWIDGETITEM_H
#define __TP_LISTWIDGETITEM_H

#include "tpString.h"
#include "tpVariant.h"
#include "tpFont.h"
#include "tpMap.h"
#include "tpGlobal.h"

class tpListWidget;
class tpListWidgetItem
{
    friend class tpListWidget;

public:
    tpListWidgetItem(tpListWidget *listview = nullptr);
    tpListWidgetItem(const tpString &text, tpListWidget *listview = nullptr);
    // tpListWidgetItem(const tpIcon &icon, const tpString &text,
    //                       tpListWidget *listview = nullptr);
    tpListWidgetItem(const tpListWidgetItem &other);

    tpListWidget *listWidget() const { return view_; }

    void setSelected(bool select);
    bool isSelected() const;

    // inline void setHidden(bool hide);
    // inline bool isHidden() const;

    tpString text() const;
    void setText(const tpString &text);

    // inline tpIcon icon() const;
    // inline void setIcon(const tpIcon &icon);

    tpString statusTip() const;
    void setStatusTip(const tpString &statusTip);

    tpString toolTip() const;
    void setToolTip(const tpString &toolTip);

    tpFont font() const;
    void setFont(const tpFont &font);

    tinyPiX::AlignmentFlag textAlignment();
    void setTextAlignment(tinyPiX::AlignmentFlag alignment);

    // virtual void setBackgroundColor(const tpColor &color)

    // tpBrush background() const;
    // void setBackground(const tpBrush &brush);

    // void setTextColor(const QColor &color);

    // ItpSize sizeHint() const;
    // void setSizeHint(const ItpSize &size);

    virtual tpVariant data(int role);
    virtual void setData(int role, const tpVariant &value);

    // QListWidgetItem &operator=(const QListWidgetItem &other);

private:
    tpListWidget* view_;
    tpString text_;

    tpFont itemFont_;

    tpMap<int, tpVariant> itemDataMap_;
    tinyPiX::AlignmentFlag alignment_;

    bool select_;
};

#endif // TP_LISTWIDGETITEM_H