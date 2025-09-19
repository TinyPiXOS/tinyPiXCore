#ifndef __TP_CSS_DATA_H
#define __TP_CSS_DATA_H

#include "TpHash.h"
#include "TpString.h"
#include "TpGradient.h"

TP_DEF_VOID_TYPE_VAR(ITpCssDataData);
/// @brief CSS样式数据容器类
class TpCssData
{
public:
    /// @brief 构造函数
    /// @param cssDataMap CSS属性键值对映射表
    TpCssData(const TpHash<TpString, TpString> &cssDataMap);

    /// @brief 析构函数
    ~TpCssData();

    /// @brief 获取元素宽度
    /// @return 宽度值(px)
    int32_t width();
    /// @brief 获取元素最小宽度
    /// @return 最小宽度值(px)
    int32_t minimumWidth();
    /// @brief 获取元素最大宽度
    /// @return 最大宽度值(px)
    int32_t maximumWidth();

    /// @brief 获取元素高度
    /// @return 高度值(px)
    int32_t height();
    /// @brief 获取元素最小高度
    /// @return 最小高度值(px)
    int32_t minimumHeight();
    /// @brief 获取元素最大高度
    /// @return 最大高度值(px)
    int32_t maximumHeight();

    /// @brief 获取字体颜色
    /// @return 颜色值(RGB格式)
    /// @note 如果颜色是渐变色，则返回渐变色的第一个颜色值
    int32_t color();
    /// @brief 检查字体颜色是否为渐变色
    /// @return true-渐变色, false-纯色
    bool colorIsGradient();
    /// @brief 获取字体颜色渐变对象
    /// @return 渐变对象指针，如果不是渐变色则返回nullptr
    TpGradient *colorGradiant();

    /// @brief 获取子标题字体颜色
    /// @return 颜色值(RGB格式)
    int32_t subColor();
    /// @brief 检查子标题字体颜色是否为渐变色
    /// @return true-渐变色, false-纯色
    bool subColorIsGradient();
    /// @brief 获取子标题字体颜色渐变对象
    /// @return 渐变对象指针，如果不是渐变色则返回nullptr
    TpGradient *subColorGradiant();

    /// @brief 获取背景颜色
    /// @return 颜色值(RGB格式)
    int32_t backgroundColor();
    /// @brief 设置背景颜色
    /// @param color 颜色值(RGB格式)
    void setBackgroundColor(const int32_t &color);
    /// @brief 检查背景颜色是否为渐变色
    /// @return true-渐变色, false-纯色
    bool backgroundColorIsGradient();
    /// @brief 获取背景颜色渐变对象
    /// @return 渐变对象指针，如果不是渐变色则返回nullptr
    TpGradient *backgroundColorGradiant();
    /// @brief 设置背景渐变颜色
    /// @param color 渐变对象指针
    void setBackgroundColor(TpGradient *color);

    /// @brief 获取边框颜色
    /// @return 颜色值(RGB格式)
    int32_t borderColor();
    /// @brief 设置边框颜色
    /// @param color 颜色值(RGB格式)
    void setBorderColor(const int32_t &color);
    /// @brief 检查边框颜色是否为渐变色
    /// @return true-渐变色, false-纯色
    bool borderColorIsGradient();
    /// @brief 获取边框颜色渐变对象
    /// @return 渐变对象指针，如果不是渐变色则返回nullptr
    TpGradient *borderColorGradiant();
    /// @brief 设置边框渐变颜色
    /// @param color 渐变对象指针
    void setBorderColor(TpGradient *color);

    /// @brief 获取边框宽度
    /// @return 边框宽度值(px)
    int32_t borderWidth();

    /// @brief 获取字体大小
    /// @return 字体大小值(px)
    int32_t fontSize();

    /// @brief 获取内部元素间距
    /// @return 间距值(px)
    int32_t gap();

    /// @brief 获取内边距（所有方向）
    /// @return 内边距值(px)
    /// @note 如果设置了单独方向的内边距，则优先使用单独设置的值
    int32_t padding();

    /// @brief 获取左侧内边距
    /// @return 左侧内边距值(px)
    /// @note 如果设置了padding()值，则优先使用padding()值
    int32_t paddingLeft();

    /// @brief 获取右侧内边距
    /// @return 右侧内边距值(px)
    /// @note 如果设置了padding()值，则优先使用padding()值
    int32_t paddingRight();

    /// @brief 获取顶部内边距
    /// @return 顶部内边距值(px)
    /// @note 如果设置了padding()值，则优先使用padding()值
    int32_t paddingTop();

    /// @brief 获取底部内边距
    /// @return 底部内边距值(px)
    /// @note 如果设置了padding()值，则优先使用padding()值
    int32_t paddingBottom();

    /// @brief 获取圆角半径
    /// @return 圆角半径值(px)
    uint32_t roundCorners();

    /// @brief 设置圆角半径
    /// @param corners 圆角半径值(px)
    void setRoundCorners(const uint32_t &corners);

    /// @brief 获取图标大小
    /// @return 图标大小值(px)
    int32_t iconSize();

    /// @brief 获取图标背景颜色
    /// @return 颜色值(RGB格式)
    int32_t iconBackground();

    /// @brief 检查图标背景颜色是否为渐变色
    /// @return true-渐变色, false-纯色
    bool iconBackgroundIsGradient();

    /// @brief 获取图标背景颜色渐变对象
    /// @return 渐变对象指针，如果不是渐变色则返回nullptr
    TpGradient *iconBackgroundGradiant();

private:
    ITpCssDataData *data_;
};

#endif