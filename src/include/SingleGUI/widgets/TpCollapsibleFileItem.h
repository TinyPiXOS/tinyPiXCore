#ifndef __TP_COLLAPSIBLE_FILE_ITEM_H
#define __TP_COLLAPSIBLE_FILE_ITEM_H

#include "TpChildWidget.h"
#include "TpEvent.h"
#include "TpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpCollapsibleFileItemData);

/// @brief 可折叠文件窗口的单个文件单元
class TpCollapsibleFileItem
    : public TpChildWidget
{
public:
    TpCollapsibleFileItem(TpChildWidget *parent = nullptr);
    ~TpCollapsibleFileItem();

    /// @brief 设置是否显示可勾选状态
    /// @param enable true显示勾选
    void setSelectEable(const bool &enable);

    /// @brief 设置文件名称
    /// @param name 文件名字符串
    void setName(const TpString &name);

    /// @brief 设置文件绝对路径
    /// @param filePath
    void setPath(const TpString &filePath);

    /// @brief 获取当前item对应的文件/文件夹绝对路径
    /// @return
    TpString path();

    /// @brief 获取item勾选状态
    /// @return
    bool selected();

    /// @brief 设置是否选中
    /// @param selected 选中状态
    void setSelected(const bool &selected);

public
signals:
    /// @brief 点击信号，鼠标释放时触发
    /// @param TpCollapsibleFileItem* 点击的item
    declare_signal(onClicked, TpCollapsibleFileItem *);

protected:
    virtual TpString pluginType() { return TO_STRING(TpCollapsibleFileItem); }

    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;
    virtual bool onLeaveEvent(TpObjectLeaveEvent *event) override;
    virtual bool onMoveEvent(TpObjectMoveEvent *event) override;
    virtual bool onResizeEvent(TpObjectResizeEvent *event) override;
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

private:
    ItpCollapsibleFileItemData *data_;
};

#endif