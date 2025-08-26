#ifndef __TP_CURSOR_H
#define __TP_CURSOR_H

#include "TpUtils.h"

/// @brief 鼠标工具类 暂未实现
class TpCursor
{
public:
    TpCursor();
     ~TpCursor();

    /// @brief 获取鼠标当前坐标
    /// @return 返回鼠标坐标
    static ItpPoint pos();
};

#endif