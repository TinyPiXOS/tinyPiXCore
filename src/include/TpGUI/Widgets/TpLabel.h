#ifndef __TP_VTEXT_LABEL_H
#define __TP_VTEXT_LABEL_H

#include "TpWidget.h"
#include <string>

TP_DEF_VOID_TYPE_VAR(ITpLabelData);

class TpFont;
class TpLabel : public TpWidget
{
public:
    TpLabel(TpWidget *parent = nullptr);
    TpLabel(const TpString &text, TpWidget *parent = nullptr);
    virtual ~TpLabel();

public:
    /// @brief 设置是否自动根据文本宽度调整宽高
    /// @param enable 
    virtual void setAutoFit(bool enable = false);

    /// @brief 设置label根据文本长度和大小，自动实现换行
    /// @param wrap 是否自动换行
    void setWordWrap(bool wrap);

public:
    virtual void setRect(const TpRect &rect);
    virtual void setRect(int32_t x, int32_t y, int32_t w, int32_t h);

public:
    /// @brief 设置显示文本
    /// @param text 文本字符串
    virtual void setText(const TpString &text);

    /// @brief 获取显示文本
    /// @return 文本字符串
    TpString text() const;

public:
    /// @brief 获取文本字体指针
    /// @return 字体指针
    virtual TpFont *font();

public:
    /// @brief 设置文本居中模式
    /// @param align 模式枚举值
    virtual void setAlign(const Tp::Alignment align = Tp::AlignLeft);

public:
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;

protected:
    /// @brief 组件类名，子类实现，返回子类类名字符串，用于匹配CSS中对应样式
    /// @return 类名字符串
    virtual TpString pluginType() override { return TO_STRING(TpLabel); }

private:
    ITpLabelData *data_;
};

#endif
