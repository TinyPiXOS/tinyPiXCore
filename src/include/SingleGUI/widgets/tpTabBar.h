#ifndef __TP_TAB_BAR_H
#define __TP_TAB_BAR_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include <tpString.h>

TP_DEF_VOID_TYPE_VAR(ItpTabBarData);
/// @brief 底部菜单,宽度设置暂时无效，跟随父窗口宽度变化
class tpTabBar : public tpChildWidget
{
public:
    tpTabBar(tpChildWidget *parent = nullptr);

    virtual ~tpTabBar();

    /// @brief 添加一个按钮
    /// @param text 按钮文本
    /// @return 索引值
    uint32_t addButton(const tpString &text);
    /// @brief 添加一个按钮
    /// @param text 按钮文本
    /// @param normalIconPath 正常状态的按钮图标
    /// @param selectIconPath 选中状态的按钮图标
    /// @return 索引值
    uint32_t addButton(const tpString &text, const tpString &normalIconPath, const tpString &selectIconPath);

    /// @brief 指定索引插入一个按钮，下标从0开始
    /// @param text 插入索引
    /// @param text 插入文本
    /// @return 插入索引
    uint32_t insertButton(const uint32_t &index, const tpString &text);

    /// @brief 设置按钮的正常态和选中态图标
    /// @param buttonIndex 索引值
    /// @param normalIconPath 正常状态的按钮图标
    /// @param selectIconPath 选中状态的按钮图标
    void setIcon(const uint32_t &buttonIndex, const tpString &normalIconPath, const tpString &selectIconPath);

    /// @brief 指定索引获取tab标题文本
    /// @param index 索引
    /// @return 文本
    tpString tabText(int32_t index) const;
    /// @brief 指定索引设置tab标题文本
    /// @param index 索引值
    /// @param text 标题文本
    void setTabText(int32_t index, const tpString &text);

    /// @brief 设置当前选中的按钮索引
    /// @param buttonId 按钮ID
    void setCurrentIndex(const uint32_t &buttonIndex);
    /// @brief 获取当前选中按钮的索引
    /// @return 索引值
    uint32_t currendIndex();

    /// @brief 指定索引移除按钮
    /// @param index 索引
    void removeButton(const uint32_t &index);

    /// @brief 获取按钮总数
    /// @return 总数量
    uint32_t count();

    /// @brief 清除所有tab选项，所有指针会被释放
    void clear();

public:
	virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpTabBar); }

public
signals:
    declare_signal(indexChanged, uint32_t);

private:
    void caculateButtonSize();

private:
    ItpTabBarData *data_;
};

#endif
