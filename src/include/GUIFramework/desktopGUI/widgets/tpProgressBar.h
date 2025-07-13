#ifndef __DESKTOP_GUI_PROGRESS_BAR_H
#define __DESKTOP_GUI_PROGRESS_BAR_H

#include "tpChildWidget.h"
#include "tpEvent.h"
#include "tpSignalSlot.h"
#include "GUIFramework/desktopGUI/FrameworkGlobal.h"

DESKTOP_GUI_NAMESPACE_BEGIN

// TP_DEF_VOID_TYPE_VAR(ItpProgressBarData);

class tpProgressBar
    : public tpChildWidget
{
public:
    tpProgressBar(tpChildWidget* parent);
    ~tpProgressBar();

    // 设置当前进度文本是否显示
    void setTextVisible(const bool &visible);

    // 设置最大最小值，根据value计算百分比
    void setMax(const uint32_t &max);
    void setMin(const uint32_t &min);

    void setValue(const uint32_t &value);

    // 设置背景色和填充色
    void setBackgroundColor(const int32_t &color);
    void setFillColor(const int32_t &color);

    /// @brief 设置是否无边框线，默认为无边框
    /// @param noBorder
    void setNoBorder(const bool &noBorder = true);

    /// @brief 设置边框颜色
    /// @param color
    void setBorderColor(const int32_t &color);

protected:
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

private:
    bool textVisible_;

    uint32_t max_;
    uint32_t min_;
    uint32_t value_;

    int32_t bgColor_;
    int32_t fillColor_;

    bool isNoBorder_;
    int32_t borderColor_;
};

DESKTOP_GUI_NAMESPACE_END

#endif
