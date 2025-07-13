#ifndef __TP_LINEEDIT_H
#define __TP_LINEEDIT_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpFont.h"

TP_DEF_VOID_TYPE_VAR(ItpLineEditData);

class tpLineEdit : public tpChildWidget
{
public:
    tpLineEdit(tpChildWidget *parent = nullptr);
    virtual ~tpLineEdit();

    /// @brief 获取输入框字体
    /// @return 字体指针
    tpFont *font();

    /// @brief 设置文本
    /// @param text 文本字符串
    void setText(const tpString &text);

    /// @brief 获取输入框当前输入文本
    /// @return 当前输入框的文本
    tpString text();

    /// @brief 设置输入框的图标，给入空则图标不显示
    /// @param iconPath 图标绝对路径
    void setIcon(const tpString &iconPath);

    /// @brief 清空输入框当前文本
    void clear();

    /// @brief 设置输入框提示文本
    /// @param text 提示文本字符串
    void setPlaceholderText(const tpString &text);

    /// @brief 获取输入框提示文本
    /// @return 文本字符串
    tpString placeholderText();

    /// @brief 设置文本居中模式；当前仅支持水平靠左、靠右、居中
	/// @param align 模式枚举值
	void setAlign(const tinyPiX::AlignmentFlag align = tinyPiX::AlignLeft);

    /// @brief 设置是否只读
    /// @param readOnly true为只读模式；false为编辑模式
    void setReadOnly(const bool& readOnly);

protected:
	virtual void virtualKeyboardInput(const tinyPiX::VirtualKeyboardInputType& type, const tpString& text) override;

    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

    virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;

    virtual bool onFocusEvent(tpObjectFocusEvent *event) override;

    virtual bool onKeyPressEvent(tpKeyboardEvent *event) override;

    virtual bool onMousePressEvent(tpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;

    virtual bool onMouseMoveEvent(tpMouseEvent *event) override;

    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpLineEdit); }

private:
    void insertCharacter(const tpString& insertStr);

    int mousePosToCursorPos(int mouseX);

    void copySelectedText();

    void pasteText();

    void deleteSelectedText();

    void caculateIconPos();

    void updateTextOffset();

    // 计算对齐偏移量
    uint32_t cacualteAlignOffset(const uint32_t& visibleTextWidth);

    // 处理backspace按键事件
    void dealBackSpace();

private:
    ItpLineEditData *data_;
};

#endif