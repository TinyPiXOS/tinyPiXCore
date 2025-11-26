#ifndef __TP_WIDGETS_DATA_H
#define __TP_WIDGETS_DATA_H

/*
 *  小组件相关交互数据定义；小组件暂未实现
 */

#include <TpNetDataGlobal.h>

/// @brief 小组件初始化消息；用于上层应用通知小组件渲染区域
static const char *TpInitWidgetsKey = "TinyPiX.TpInitWidgetsKey";
struct TpInitWidgets : NetDataBase
{
    int32_t width = 0;
    int32_t height = 0;

    int32_t depth = 0;
    int32_t Rmask = 0;
    int32_t Gmask = 0;
    int32_t Bmask = 0;
    int32_t Amask = 0;

    TpInitWidgets() : NetDataBase(TpInitWidgetsKey)
    {
    }

    StructSerializeFunc;
};
STRUCT_PACKAGE(TpInitWidgets, dataHead_, width, height);

/// @brief 上层应用通知小组件需要重新绘制并发送渲染指令
static const char *TpPaintWidgetsKey = "TinyPiX.TpPaintWidgetsKey";
struct TpPaintWidgets : NetDataBase
{
    TpPaintWidgets() : NetDataBase(TpPaintWidgetsKey)
    {
    }

    StructSerializeFunc;
};
STRUCT_PACKAGE(TpPaintWidgets, dataHead_);

/// @brief 小组件通知上层应用组件已绘制完毕；开始渲染
static const char *TpUpdateWidgetsKey = "TinyPiX.TpUpdateWidgetsKey";
struct TpUpdateWidgets : NetDataBase
{
    uint64_t sharedId = 0;

    TpUpdateWidgets() : NetDataBase(TpUpdateWidgetsKey)
    {
    }

    StructSerializeFunc;
};
STRUCT_PACKAGE(TpUpdateWidgets, dataHead_, sharedId);

#endif