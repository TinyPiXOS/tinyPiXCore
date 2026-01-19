#ifndef __TP_TAB_BAR_H
#define __TP_TAB_BAR_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include <TpString.h>

TP_DEF_VOID_TYPE_VAR(ITpTabBarData);
/// @brief 底部菜单,宽度设置暂时无效，跟随父窗口宽度变化
class TpTabBar : public TpWidget
{
public:
    TpTabBar(TpWidget *parent = nullptr);

    virtual ~TpTabBar();

    /// @brief 添加一个按钮
    /// @param text 按钮文本
    /// @return 索引值
    uint32_t addButton(const TpString &text);
    /// @brief 添加一个按钮
    /// @param text 按钮文本
    /// @param normalIconPath 正常状态的按钮图标
    /// @param selectIconPath 选中状态的按钮图标
    /// @return 索引值
    uint32_t addButton(const TpString &text, const TpString &normalIconPath, const TpString &selectIconPath);

    /// @brief 指定索引插入一个按钮，下标从0开始
    /// @param text 插入索引
    /// @param text 插入文本
    /// @return 插入索引
    uint32_t insertButton(const uint32_t &index, const TpString &text);

    /// @brief 设置按钮的正常态和选中态图标
    /// @param buttonIndex 索引值
    /// @param normalIconPath 正常状态的按钮图标
    /// @param selectIconPath 选中状态的按钮图标
    void setIcon(const uint32_t &buttonIndex, const TpString &normalIconPath, const TpString &selectIconPath);

    /// @brief 指定索引获取tab标题文本
    /// @param index 索引
    /// @return 文本
    TpString tabText(int32_t index) const;
    /// @brief 指定索引设置tab标题文本
    /// @param index 索引值
    /// @param text 标题文本
    void setTabText(int32_t index, const TpString &text);

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
	virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpTabBar); }

public
signals:
    declare_signal(indexChanged, uint32_t);

private:
    void caculateButtonSize();

private:
    ITpTabBarData *data_;
};

#endif
