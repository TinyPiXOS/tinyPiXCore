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
    TpMessageBox(const char *type = "tinyPiX_USE_Float");

    virtual ~TpMessageBox();

    /// @brief 模态显示消息对话框
    /// @return 返回点击ID索引
    uint32_t exec();

    /// @brief 设置消息框类型
    /// @param type 类型枚举
    void setMessageType(const MessageType &type);

    /// @brief 设置消息框按钮列表，最大最好不要超过四个，否则显示效果受影响
    /// @param buttonList 按钮文本列表
    void setButtonList(const TpVector<TpString> &buttonList);

public:
    static uint32_t information(const TpString &text, const TpString &btnText = "确认");
    static uint32_t question(const TpString &text, const TpString &yesBtnText = "确认", const TpString &cancleBtnText = "取消");
    static uint32_t error(const TpString &text, const TpString &yesBtnText = "确认");
    static uint32_t warning(const TpString &text, const TpString &yesBtnText = "确认");

public:
    /// @brief 设置按钮文本
    /// @param text 文本内容
    virtual void setText(const TpString &text);

    /// @brief 获取按钮文本字体
    /// @return 字体指针
    virtual TpFont *font();

public
signals:

protected:
    virtual void setVisible(bool visible = true) override;

protected:
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;

    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpMessageBox); }

private:
    ItpMessageBoxData *data_;
};

#endif
