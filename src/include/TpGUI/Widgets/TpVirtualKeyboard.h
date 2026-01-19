#ifndef __TP_VIRTUAL_KEYBOARD_H
#define __TP_VIRTUAL_KEYBOARD_H

#include "TpDialog.h"
#include "TpSignalSlot.h"
#include "TpEvent.h"
#include <TpCore.h>

class TpButton;

/// @brief 虚拟键盘;使用TpApp相关接口获取实例
TP_DEF_VOID_TYPE_VAR(ITpVirtualKeyboardData);
class TpVirtualKeyboard
    : public TpDialog
{
public:
    TpVirtualKeyboard();
    ~TpVirtualKeyboard();

    /// @brief 是否是中文输入状态
    /// @return 中文状态返回true，否则返回false
    bool isChinese();

    /// @brief 切换是否是中文输入
    /// @param isChinese 是否是中文输入
    /// @return 切换结果
    bool switchChinese(const bool &isChinese);

    /// @brief 是否是输入大写模式
    /// @return 大写模式返回true，否则返回false
    bool isUpper();

    /// @brief 软键盘使用show方法显示
    virtual void show() override;

public
signals:
    /// @brief 输入拼音字符
    /// @param ch 拼音字符
    declare_signal(inputPinyin, const TpString &);

    /// @brief 完成输入中文
    /// @param ch 中文文本
    declare_signal(finishChinese, const TpString &);

    /// @brief 删除
    declare_signal(deleteSymbol);

    /// @brief 切换中英文
    /// @param _isChinese true为中文
    declare_signal(chEnSwitch, const bool &);

    ///// @brief输入字符（中文字符或者英文字符、或者数字）
    /// @param ch 字符
    declare_signal(inputCharacter, const TpString &);

protected:
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

    virtual bool onPaintEvent(TpPaintEvent *event) override;

    virtual bool onKeyPressEvent(TpKeyboardEvent *event) override;
    virtual bool onKeyReleaseEvent(TpKeyboardEvent *event) override;

    virtual bool onResizeEvent(TpResizeEvent *event) override;

private:
    void configWindowLayout();

    void configSignal();

    void pressLetterButton(TpButton *pressBtn);

private:
    ITpVirtualKeyboardData *data_;
};

#endif