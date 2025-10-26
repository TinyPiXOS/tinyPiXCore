#ifndef __TP_WIDGETS_GATEWAY_H
#define __TP_WIDGETS_GATEWAY_H

#include "TpUtils.h"
#include "TpPainter.h"
#include "TpString.h"
#include "TpGateway.h"
#include "TpNetDataGlobal.h"

TP_DEF_VOID_TYPE_VAR(ITpWidgetsGatewayData);
/// @brief 系统组件相关回调接口类；每个小组件只应继承一次该接口类
class TpWidgetsGateway : public TpGateway
{
public:
    TpWidgetsGateway() = delete;
    /// @brief 构造函数
    /// @param widgetUuid 组件UUID
    TpWidgetsGateway(const TpString &widgetUuid);
    virtual ~TpWidgetsGateway();

    /// @brief 小组件窗口尺寸变化；会通知新的画笔对象；使用新画笔对象绘制
    /// @param painter 新的画笔对象
    virtual void widgetResizeEvent(TpPainter *painter) = 0;

    /// @brief 上层通知刷新事件；需要重绘并发送刷新请求
    virtual void onPaintEvent() = 0;

    /// @brief 刷新指令；绘制完毕后通知上层应用刷新小组件显示
    void update();

    virtual void recvData(const char *topic, const void *data, const uint32_t &size) override final;

private:
    ITpWidgetsGatewayData *data_;
};

#endif
