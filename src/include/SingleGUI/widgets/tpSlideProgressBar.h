
#ifndef __TP_SLIDE_PROGRESSBAR_H
#define __TP_SLIDE_PROGRESSBAR_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpEvent.h"
#include "tpString.h"

TP_DEF_VOID_TYPE_VAR(ItpSlideProgressBarData);
/// @brief 滑动进度条，可用手势或鼠标拖拽进度
class tpSlideProgressBar : public tpChildWidget
{
public:
    tpSlideProgressBar(tpChildWidget *parent);
    ~tpSlideProgressBar();

    /// @brief 设置图标文件路径
    /// @param iconPath
    void setIcon(const tpString &iconPath);

    /// @brief 设置进度条最大最小值
    /// @param minValue 最小值
    /// @param maxValue 最大值
    void setRange(const int32_t &minValue, const int32_t &maxValue);

    /// @brief 设置进度条当前进度
    /// @param value 进度值
    void setValue(const int32_t &value);

    /// @brief 获取当前值
    /// @return 进度值
    int32_t value();

public
signals:
    declare_signal(onValueChanged, int32_t);

public:
    virtual void setParent(tpObject *parent) override;

    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(tpMouseEvent *event) override;
    virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;

    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual void onThemeChangeEvent(tpThemeChangeEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpSlideProgressBar); }

private:
    ItpSlideProgressBarData *data_;
};

#endif
