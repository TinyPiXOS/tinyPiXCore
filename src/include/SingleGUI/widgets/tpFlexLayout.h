#ifndef __TP_FLEX_LAYOUT_H
#define __TP_FLEX_LAYOUT_H

#include "tpLayout.h"

TP_DEF_VOID_TYPE_VAR(ItpFlexLayoutData);

/// @brief 响应式布局，会根据窗口宽高，自动调整一行/列显示控件数量，不会修改控件大小,但控件必须设置大小，如果大小为0则不可见
class tpFlexLayout
    : public tpLayout
{
public:
    /// @brief 方向布局
    enum FlexDirection
    {
        Row,          // 从左到右
        RowReverse,   // 从右到左
        Column,       // 从上到下
        ColumnReverse // 从下到上
    };

    /// @brief 主轴对齐
    enum JustifyContent
    {
        MainFlexStart,    // 默认, 从主轴 开始位置 开始
        MainFlexEnd,      // 从主轴 结束位置 开始
        MainCenter,       // 居中对齐
        MainSpaceBetween, // 两端对齐,两边贴着
        MainSpaceEvenly,  // 分散对齐,间距相同
        MainSpaceAround   // 分散对齐,两边是中间的一半
    };

    /// @brief 交叉轴对齐方式
    enum AlignItems
    {
        CrossFlexStart, // 顶部对齐
        CrossFlexEnd,   // 底部对齐
        CrossCenter     // 居中对齐
    };

    /// @brief 多行交叉轴对齐方式
    enum AlignContent
    {
        Stretch,      // 默认,拉伸，与交叉轴对齐
        FlexStart,    // 顶部对齐
        FlexEnd,      // 底部对齐
        Center,       // 居中对齐
        SpaceBetween, // 分散对齐,上下贴着  暂未实现
        SpaceEvenly,  // 分散对齐,上中下平均 暂未实现
        SpaceAround   // 分散对齐,上下是中一半 暂未实现
    };

public:
    tpFlexLayout(tpChildWidget *parent = nullptr);

    virtual ~tpFlexLayout();

    /// @brief 向布局内添加窗口
    /// @param childWidget 窗口指针
    void addWidget(tpChildWidget *childWidget);

    /// @brief 向布局内添加布局
    /// @param layout 布局指针
    void addLayout(tpLayout *layout);

    /// @brief 插入一个窗口
    /// @param index 插入索引，从0开始
    /// @param widget 窗口指针
    void insertWidget(uint32_t index, tpChildWidget *widget);

    /// @brief 插入一个布局
    /// @param index 插入索引，从0开始
    /// @param layout 布局指针
    void insertLayout(uint32_t index, tpLayout *layout);

    /// @brief 从布局移除一个指定widget，不会释放指针
    /// @param widget widget指针
    virtual void removeWidget(tpChildWidget *widget) override;

    /// @brief 从布局移除一个子布局，不会释放指针
    /// @param layout 布局指针
    virtual void removeLayout(tpLayout *layout) override;

    /// @brief 设置布局方向
    /// @param direction 方向枚举
    void setFlexDirection(const FlexDirection &direction);

    /// @brief 设置主轴对齐方式
    /// @param justify 主轴对齐方式
    void setJustifyContent(const JustifyContent &justify);

    /// @brief 设置交叉轴对齐方式
    /// @param alignItems 对齐方式
    void setAlignItems(const AlignItems &alignItems);

    /// @brief 设置多行交叉轴对齐方式，注意设置后会覆盖交叉轴对齐样式
    /// @param alignContent 交叉轴对齐方式
    void setAlignContent(const AlignContent &alignContent);

    /// @brief 布局更新
    virtual void update() override;

    /// @brief 获取布局当前行数
    /// @return 布局行数
    virtual uint32_t rowCount() override;

    /// @brief 获取布局当前列数
    /// @return 布局列数
    virtual uint32_t columnCount() override;

    /// @brief 清空布局内所有子部件;使用children获取item后移除后再clear
    virtual void clear() override;

    /// @brief 获取子控件列表
    /// @return 控件指针列表
    virtual tpVector<tpObject *> children() override;

private:
    ItpFlexLayoutData *data_;
};

#endif
