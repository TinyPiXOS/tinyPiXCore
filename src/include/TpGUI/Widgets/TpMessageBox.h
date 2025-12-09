#ifndef __TP_MESSAGE_BOX_H
#define __TP_MESSAGE_BOX_H

#include "TpDialog.h"
#include "TpSignalSlot.h"
#include "TpString.h"
#include "TpVector.h"
#include "TpFont.h"

TP_DEF_VOID_TYPE_VAR(ItpMessageBoxData);
/// @brief 消息提示框
class TpMessageBox : public TpDialog
{
public:
    enum MessageType
    {
        Information,
        Question,
        Warning,
        Error
    };

public:
    TpMessageBox(MessageType type = TpMessageBox::Information);
    TpMessageBox(const TpString &text, MessageType type = TpMessageBox::Information);
    virtual ~TpMessageBox();

    /// @brief 模态显示消息对话框
    /// @return 返回点击ID索引
    void exec() override;

    /// @brief 设置按钮文本
    /// @param text 文本内容
    virtual void setText(const TpString &text);

    /// @brief 设置消息框类型；内部根据类型自动生成交互按钮
    /// @param type 类型枚举
    void setMessageType(MessageType type);

    /// @brief 设置消息框按钮列表，最大最好不要超过四个
    /// @param buttonList 按钮文本列表
    void setButtonList(const TpVector<TpString> &buttonList);

public
signals:
    /// @brief 消息框关闭信号
    /// @param int32_t 用户点击的交互按钮索引，从0开始
    declare_signal(onClose, int32_t);

protected:
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpMessageBox); }

private:
    ItpMessageBoxData *data_;
};

#endif
