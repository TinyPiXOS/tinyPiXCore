#ifndef __TP_VRADIO_BUTTON_H
#define __TP_VRADIO_BUTTON_H

#include "TpWidget.h"
#include "TpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpRadioButtonData);

class TpColors;
class TpFont;
class TpString;
/// @brief 单选按钮
class TpRadioButton : public TpWidget
{
public:
    TpRadioButton(TpWidget *parent = nullptr);
    TpRadioButton(const TpString &text, TpWidget *parent = nullptr);

    virtual ~TpRadioButton();

public:
    /// @brief 根据字体宽度和高度绘制，当设置时对齐将无效
    /// @param enable 是否自动调整
    virtual void setAutoFit(bool enable = false);

    /// @brief 设置按钮与文本间距值
    /// @param space 间距
    virtual void setSpacing(uint32_t space = 1);

    virtual void setRect(int32_t x, int32_t y, int32_t w, int32_t h) override;

public:
    /// @brief 设置按钮文本
    /// @param text 文本字符串
    virtual void setText(const TpString &text);
    /// @brief 获取文本
    /// @return 文本字符串
    TpString text() const;

    virtual TpFont *font();

    virtual TpString pluginType() override { return TO_STRING(TpRadioButton); }

public
signals:
    declare_signal(onClicked, bool);

protected:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event);

private:
    ItpRadioButtonData *data_;
};

#endif
