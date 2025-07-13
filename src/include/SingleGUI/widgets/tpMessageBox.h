#ifndef __TP_MESSAGE_BOX_H
#define __TP_MESSAGE_BOX_H

#include "tpDialog.h"
#include "tpSignalSlot.h"
#include "tpString.h"
#include "tpVector.h"
#include "tpFont.h"

TP_DEF_VOID_TYPE_VAR(ItpMessageBoxData);
/// @brief 消息提示框
class tpMessageBox : public tpDialog
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
    tpMessageBox(const char *type = "tinyPiX_USE_Float");

    virtual ~tpMessageBox();

    /// @brief 模态显示消息对话框
    /// @return 返回点击ID索引
    uint32_t exec();

    /// @brief 设置消息框类型
    /// @param type 类型枚举
    void setMessageType(const MessageType &type);

    /// @brief 设置消息框按钮列表，最大最好不要超过四个，否则显示效果受影响
    /// @param buttonList 按钮文本列表
    void setButtonList(const tpVector<tpString> &buttonList);

public:
    static uint32_t information(const tpString &text, const tpString &btnText = "确认");
    static uint32_t question(const tpString &text, const tpString &yesBtnText = "确认", const tpString &cancleBtnText = "取消");
    static uint32_t error(const tpString &text, const tpString &yesBtnText = "确认");
    static uint32_t warning(const tpString &text, const tpString &yesBtnText = "确认");

public:
    /// @brief 设置按钮文本
    /// @param text 文本内容
    virtual void setText(const tpString &text);

    /// @brief 获取按钮文本字体
    /// @return 字体指针
    virtual tpFont *font();

public
signals:

protected:
    virtual void setVisible(bool visible = true) override;

protected:
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpMessageBox); }

private:
    ItpMessageBoxData *data_;
};

#endif
