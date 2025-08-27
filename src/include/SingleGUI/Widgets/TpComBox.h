#ifndef __TP_LIST_MODE_COMBOX_H
#define __TP_LIST_MODE_COMBOX_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"
#include "TpVariant.h"

TP_DEF_VOID_TYPE_VAR(ItpComboxData);
/// @brief 下拉框组件
class TpComBox : public TpChildWidget
{
public:
    TpComBox(TpChildWidget *parent = nullptr);
    virtual ~TpComBox();

public:
    /// @brief 设置下拉框描述文本
    /// @param text 文本内容
    virtual void setTitle(const TpString &text);

    /// @brief 添加一个item
    /// @param text item文本
    /// @param itemData item绑定数据，可以不给入
    virtual void addItem(const TpString &text, const TpVariant &itemData = TpVariant());

    /// @brief 获取当前选中的item的索引
    /// @return 索引下标，从0开始
    uint32_t currentIndex();

    /// @brief 获取当前选中的item的文本
    /// @return 文本字符串
    TpString currentText();

    /// @brief 获取当前选中的item的自定义数据
    /// @return 数据
    TpVariant currentData();

    /// @brief 清除所有Item
    void clear();

    virtual void setMinumumHeight(const uint32_t &height) override;

public
signals:
    /// @brief 选中项切换事件
    /// @param uint32_t 当前下标索引
    declare_signal(currentIndexChanged, uint32_t);

    /// @brief 选中项切换时间
    /// @param const TpString& 当前选中项文本
    declare_signal(currentTextChanged, const TpString &);

protected:
    virtual bool onResizeEvent(TpObjectResizeEvent *event) override;
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpComBox); }

private:
    ItpComboxData *data_;
};

#endif
