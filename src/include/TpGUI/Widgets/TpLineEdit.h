#ifndef __TP_LINEEDIT_H
#define __TP_LINEEDIT_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpFont.h"

TP_DEF_VOID_TYPE_VAR(ItpLineEditData);

class TpLineEdit : public TpWidget
{
public:
    TpLineEdit(TpWidget *parent = nullptr);
    virtual ~TpLineEdit();

    /// @brief 获取输入框字体
    /// @return 字体指针
    TpFont *font();

    /// @brief 设置文本
    /// @param text 文本字符串
    void setText(const TpString &text);

    /// @brief 获取输入框当前输入文本
    /// @return 当前输入框的文本
    TpString text();

    /// @brief 设置输入框的图标，给入空则图标不显示
    /// @param iconPath 图标绝对路径
    void setIcon(const TpString &iconPath);

    /// @brief 清空输入框当前文本
    void clear();

    /// @brief 设置输入框提示文本
    /// @param text 提示文本字符串
    void setPlaceholderText(const TpString &text);

    /// @brief 获取输入框提示文本
    /// @return 文本字符串
    TpString placeholderText();

    /// @brief 设置文本居中模式；
    /// @param align 模式枚举值
    void setAlign(Tp::Alignment align = Tp::AlignLeft | Tp::AlignVCenter);

    /// @brief 设置是否只读
    /// @param readOnly true为只读模式；false为编辑模式
    void setReadOnly(const bool &readOnly);

protected:
    virtual void virtualKeyboardInput(const Tp::VirtualKeyboardInputType &type, const TpString &text) override;

    virtual bool onPaintEvent(TpPaintEvent *event) override;

    virtual bool onLeaveEvent(TpLeaveEvent *event) override;

    virtual bool onFocusEvent(TpFocusEvent *event) override;

    virtual bool onKeyPressEvent(TpKeyboardEvent *event) override;

    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;

    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;

    virtual bool onResizeEvent(TpResizeEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpLineEdit); }

private:
    void insertCharacter(const TpString &insertStr);

    int32_t mousePosToCursorPos(int32_t mouseX);

    void copySelectedText();

    void pasteText();

    void deleteSelectedText();

    void caculateIconPos();

    void updateTextOffset();

    // 计算对齐偏移量
    uint32_t cacualteAlignOffset(const uint32_t &visibleTextWidth);

    // 处理backspace按键事件
    void dealBackSpace();

private:
    ItpLineEditData *data_;
};

#endif