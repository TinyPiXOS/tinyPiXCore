#ifndef __TP_DESTOP_GUI_FILELISTWIDGET_H
#define __TP_DESTOP_GUI_FILELISTWIDGET_H

#include "tpListWidgetItem.h"
#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpList.h"
#include "tpString.h"
#include "tpEvent.h"
#include "tpVariant.h"
#include "tpFont.h"
#include "GUIFramework/desktopGUI/FrameworkGlobal.h"

DESKTOP_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(IFileListWidgetData);

/// @brief 桌面UI框架下的，文件列表窗口
class tpFileListWidget
    : public tpChildWidget
{
public:
    tpFileListWidget(tpChildWidget *parent);

    ~tpFileListWidget();

    /// @brief 设置标题栏文本
    /// @param headerList 标题文本列表
    void setHeaderList(const tpList<tpString> &headerList);

    /// @brief 设置标题栏背景色
    /// @param color rgb
    void setHeaderBackColor(const int32_t &color);

    /// @brief 设置列宽占比
    /// @param strechList 每列宽度所占比值
    void setColumnStrech(const tpList<uint32_t> &strechList);

    /// @brief 添加一行数据
    /// @param rowData 当前行文本内容
    /// @param data 当前行绑定数据,该行数据的唯一标识
    void addRow(const tpList<tpString> &rowData, const tpVariant &data);

    /// @brief 对某行数据的某一列设置icon，不设置则不绘制icon
    /// @param data 指定行数据
    /// @param column 要设置icon的列，从0开始
    /// @param iconPath 设置的icon绝对路径
    void setColumnIcon(const tpVariant &data, const uint32_t &column, const tpString &iconPath);

    /// @brief 设置一行数据的背景色
    /// @param color rgb
    void setRowBackColor(const int32_t &color);

    /// @brief 设置一行数据鼠标悬停色
    /// @param color rgb
    void setRowHoverColor(const int32_t &color);

    /// @brief 设置一行数据鼠标点击色
    /// @param color rgb
    void setRowPressColor(const int32_t &color);

    /// @brief 获取标题栏字体
    /// @return 字体指针，请勿释放
    tpFont *titleFont();

    /// @brief 获取item字体
    /// @return 字体指针，请勿释放
    tpFont *itemFont();

    /// @brief 获取选中列表
    /// @return 选中行的数据
    tpList<tpVariant> selectedList();

    /// @brief 设置选择模式，默认为 tinyPiX::ExtendedSelection
    /// @param selectMode 选择模式枚举
    // void setSelectionModel(tinyPiX::ItemSelectionMode selectMode);

    /// @brief 设置标题栏高度
    /// @param height 高度值，px
    void setTitleHeight(const uint32_t &height);

    /// @brief 设置item高度
    /// @param height 高度值，px
    void setItemHeight(const uint32_t &height);

    /// @brief 清空所有当前item
    void clear();

public
signals:
    /// @brief item被点击事件，鼠标按下即触发
    /// @param tpVariant 选中行的数据
    declare_signal(itemPressed, tpVariant);
    /// @brief item双击事件
    /// @param tpVariant 选中行的数据
    declare_signal(itemDoubleClicked, tpVariant);
    /// @brief item右键事件
    /// @param tpVariant 选中行的数据
    declare_signal(itemContext, tpVariant);

    /// @brief 窗口右键事件
    /// @param ItpPoint 窗口内部坐标
    /// @param ItpPoint 全局坐标
    declare_signal(widgetContext, ItpPoint, ItpPoint);

    /// @brief item切换
    /// @param tpVariant 上一个item的数据
    /// @param tpVariant 当前item数据
    // declare_signal(currentItemChanged, tpVariant, tpVariant);

protected:
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual bool onKeyPressEvent(tpKeyboardEvent *event) override;
    virtual bool onKeyReleaseEvent(tpKeyboardEvent *event) override;

    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseDoubleClickEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(tpMouseEvent *event) override;
    virtual bool onWheelEvent(tpWheelEvent *event) override;

    virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;

    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

private:
    tpVariant queryPointIndex(const ItpPoint &point);

private:
    IFileListWidgetData *privData;
};

DESKTOP_GUI_NAMESPACE_END

#endif