#ifndef __TP_CSS_DATA_H
#define __TP_CSS_DATA_H

#include "tpHash.h"
#include "tpString.h"

/// @brief 所有距离、宽度等单位全为px
class tpCssData
{
public:
    tpCssData(const tpHash<tpString, tpString> &cssDataMap);
    ~tpCssData();

    /// @brief 宽度
    /// @return
    int32_t width();

    /// @brief 最小宽度
    /// @return 
    int32_t minimumWidth();

    /// @brief 最大宽度
    /// @return 
    int32_t maximumWidth();

    /// @brief 高度
    /// @return
    int32_t height();

    /// @brief 最小高度
    /// @return 
    int32_t minimumHeight();

    /// @brief 最小高度
    /// @return 
    int32_t maximumHeight();

    /// @brief 字体颜色
    /// @return
    int32_t color();

    /// @brief 如果有子标题，子字体颜色
    /// @return 
    int32_t subColor();

    /// @brief 背景颜色
    /// @return
    int32_t backgroundColor();
    void setBackgroundColor(const int32_t &color);

    /// @brief 边框颜色
    /// @return
    int32_t borderColor();
    void setBorderColor(const int32_t &color);

    /// @brief 获取边框宽度
    /// @return 宽度值
    int32_t borderWidth();

    /// @brief 字体大小
    /// @return 单位px
    int32_t fontSize();

    /// @brief 内部间距
    /// @return 单位px
    int32_t gap();

    /// @brief 获取距离所有边框的距离
    /// @return 单位px
    int32_t padding();

    /// @brief 距离左边框距离，如果padding有值则使用的padding的值
    /// @return 单位px
    int32_t paddingLeft();

    /// @brief 距离有边框距离，如果padding有值则使用的padding的值
    /// @return 单位px
    int32_t paddingRight();

    /// @brief 内部组件距离外边框顶部的距离，如果padding有值则使用的padding的值
    /// @return px
    int32_t paddingTop();

    /// @brief 内部组件距离外边框底部的距离，如果padding有值则使用的padding的值
    /// @return px
    int32_t paddingBottom();

    /// @brief 圆角, px
    /// @return
    uint32_t roundCorners();
    void setRoundCorners(const uint32_t &corners);

    /// @brief 图标大小
    /// @return  px
    int32_t iconSize();

    // /// @brief 内部组件距离外边框顶部的距离
    // /// @return px
    // int32_t marginTop();

    // /// @brief 内部组件距离外边框底部的距离
    // /// @return px
    // int32_t marginBottom();

    /// @brief 如果有icon，icon显示区域的背景颜色
    /// @return
    int32_t iconBackground();

private:
    tpHash<tpString, tpString> cssDataMap_;
};

#endif
