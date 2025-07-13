#ifndef __TP_COLLAPSIBLE_FILE_ITEM_H
#define __TP_COLLAPSIBLE_FILE_ITEM_H

#include "tpChildWidget.h"
#include "tpEvent.h"
#include "tpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpCollapsibleFileItemData);

/// @brief 可折叠文件窗口的单个文件单元
class tpCollapsibleFileItem
    : public tpChildWidget
{
public:
    tpCollapsibleFileItem(tpChildWidget *parent = nullptr);
    ~tpCollapsibleFileItem();

    /// @brief 设置是否显示可勾选状态
    /// @param enable true显示勾选
    void setSelectEable(const bool &enable);

    /// @brief 设置文件名称
    /// @param name 文件名字符串
    void setName(const tpString &name);

    /// @brief 设置文件绝对路径
    /// @param filePath
    void setPath(const tpString &filePath);

    /// @brief 获取当前item对应的文件/文件夹绝对路径
    /// @return
    tpString path();

    /// @brief 获取item勾选状态
    /// @return
    bool selected();

    /// @brief 设置是否选中
    /// @param selected 选中状态
    void setSelected(const bool &selected);

public
signals:
    /// @brief 点击信号，鼠标释放时触发
    /// @param tpCollapsibleFileItem* 点击的item
    declare_signal(onClicked, tpCollapsibleFileItem *);

protected:
    virtual tpString pluginType() { return TO_STRING(tpCollapsibleFileItem); }

    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(tpMouseEvent *event) override;
    virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;
    virtual bool onMoveEvent(tpObjectMoveEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;
    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

private:
    ItpCollapsibleFileItemData *data_;
};

#endif