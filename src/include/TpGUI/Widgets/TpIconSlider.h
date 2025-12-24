
#ifndef __TP_ICON_SLIDER_H
#define __TP_ICON_SLIDER_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpEvent.h"
#include "TpString.h"

TP_DEF_VOID_TYPE_VAR(ITpIconSliderData);
/// @brief 带图标的滑动进度条，可用手势或鼠标拖拽进度
class TpIconSlider : public TpWidget
{
public:
    TpIconSlider(TpWidget *parent);
    ~TpIconSlider();

    /// @brief 设置图标文件路径
    /// @param iconPath
    void setIcon(const TpString &iconPath);

    /// @brief 设置进度条最大最小值
    /// @param minValue 最小值
    /// @param maxValue 最大值
    void setRange(int32_t minValue, int32_t maxValue);

    /// @brief 设置进度条当前进度
    /// @param value 进度值
    void setValue(int32_t value);

    /// @brief 获取当前值
    /// @return 进度值
    int32_t value();

public
signals:
    declare_signal(onValueChanged, int32_t);

public:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;

    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual void onThemeChangeEvent(TpThemeChangeEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpIconSlider); }

private:
    ITpIconSliderData *data_;
};

#endif
