/*** 
 * @Author: hywang
 * @Date: 2024-05-31 09:54:54
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-17 10:59:49
 * @FilePath: /pix-singlegui/PixSingleGUI/include/SingleGUI/widgets/TpListWidgetItem.h
 * @Description: 
 * @
 * @PiXOS
 */

/***
 * @Author: hywang
 * @Date: 2024-05-31 09:54:54
 * @LastEditors: hywang
 * @LastEditTime: 2024-05-31 10:00:47
 * @FilePath: /pix-singlegui/PixSingleGUI/include/SingleGUI/widgets/TpListWidgetItem.h
 * @Description: ListWidget的单个条目
 * @
 * @PiXOS
 */

#ifndef __TP_LISTWIDGETITEM_H
#define __TP_LISTWIDGETITEM_H

#include "TpString.h"
#include "TpVariant.h"
#include "TpFont.h"
#include "TpMap.h"
#include "TpGlobal.h"

class TpListWidget;
class TpListWidgetItem
{
    friend class TpListWidget;

public:
    TpListWidgetItem(TpListWidget *listview = nullptr);
    TpListWidgetItem(const TpString &text, TpListWidget *listview = nullptr);
    // TpListWidgetItem(const TpIcon &icon, const TpString &text,
    //                       TpListWidget *listview = nullptr);
    TpListWidgetItem(const TpListWidgetItem &other);

    TpListWidget *listWidget() const { return view_; }

    void setSelected(bool select);
    bool isSelected() const;

    // inline void setHidden(bool hide);
    // inline bool isHidden() const;

    TpString text() const;
    void setText(const TpString &text);

    // inline TpIcon icon() const;
    // inline void setIcon(const TpIcon &icon);

    TpString statusTip() const;
    void setStatusTip(const TpString &statusTip);

    TpString toolTip() const;
    void setToolTip(const TpString &toolTip);

    TpFont font() const;
    void setFont(const TpFont &font);

    tinyPiX::AlignmentFlag textAlignment();
    void setTextAlignment(tinyPiX::AlignmentFlag alignment);

    // virtual void setBackgroundColor(const TpColor &color)

    // TpBrush background() const;
    // void setBackground(const TpBrush &brush);

    // void setTextColor(const QColor &color);

    // ItpSize sizeHint() const;
    // void setSizeHint(const ItpSize &size);

    virtual TpVariant data(int role);
    virtual void setData(int role, const TpVariant &value);

    // QListWidgetItem &operator=(const QListWidgetItem &other);

private:
    TpListWidget* view_;
    TpString text_;

    TpFont itemFont_;

    TpMap<int, TpVariant> itemDataMap_;
    tinyPiX::AlignmentFlag alignment_;

    bool select_;
};

#endif // TP_LISTWIDGETITEM_H