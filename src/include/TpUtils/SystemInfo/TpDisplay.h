#ifndef __TP_DISPLAY_INFO_H
#define __TP_DISPLAY_INFO_H

#include "TpVector.h"
#include "TpString.h"
#include <TpCore.h>
#include "TpList.h"
#include "TpSize.h"

TP_DEF_VOID_TYPE_VAR(ITpDisplayInfoData);
class TpDisplay
{
public:
    typedef TpList<TpDisplay *> TpDisplayInfoList;

    // 显示模式参数
    struct TpDisplayModeParam
    {
        tpUInt16 width;
        tpUInt16 height;
        float refresh;
        TpDisplayModeParam(tpUInt16 width, tpUInt16 height, float refresh) : width(width), height(height), refresh(refresh) {}
    };

    // 旋转角度
    enum TpRotate
    {
        TpRotate_0 = 0,
        TpRotate_90 = 90,
        TpRotate_180 = 180,
        TpRotate_270 = 270
    };

public:
    // TpDisplayInfo(Display *display, XRRScreenResources *resources, RROutput output, tpInt32 num);
    TpDisplay();
    TpDisplay(tpInt32 screen); // 屏幕编号
    TpDisplay(TpString &name); // 屏幕名称
    ~TpDisplay();

public:
    /// @brief 将DP值根据当前显示器分辨率转换为像素值(px)
    /// @param _dp dp值
    /// @param _screenNum 屏幕编号，给入则使用指定屏幕编号，否则则使用程序当前显示的屏幕
    /// @return 转换后的px值
    static uint32_t dp2Px(const uint32_t &_dp, const int32_t &_screenNum = -1);

    /// @brief 将SP值根据当前显示器分辨率转换为像素值(px)
    /// @param _sp sp值
    /// @param _screenNum 屏幕编号，给入则使用指定屏幕编号，否则则使用程序当前显示的屏幕
    /// @return 转换后的px值
    static uint32_t sp2Px(const uint32_t &_sp, const int32_t &_screenNum = -1);

public:
    /// @brief 获取显示器显示模式
    /// @return 显示模式列表
    TpList<TpDisplayModeParam> displayMode();
    /// @brief 获取当前显示器的名字
    /// @return 显示器名称
    TpString name();

    /// @brief 获取物理尺寸的高度
    /// @return 返回单位mm
    tpInt32 physicsHeight();
    /// @brief 获取物理尺寸的宽度
    /// @return 返回单位mm
    tpInt32 physicsWidth();

    /// @brief 获取TpWM物理尺寸高度
    /// @return 返回单位mm
    tpInt32 wmPhysicsHeight();
    /// @brief 获取TpWM物理尺寸宽度
    /// @return 返回单位mm
    tpInt32 wmPhysicsWidth();

    /// @brief 获取当前显示器分辨率高度;真机模式下与TpWM分辨率一致
    /// @return 分辨率像素高度
    tpInt32 resolutionHeight();
    /// @brief 获取当前显示器分辨率宽度;真机模式下与TpWM分辨率一致
    /// @return 分辨率像素宽度
    tpInt32 resolutionWidth();

    /// @brief 获取TpWM的分辨率的宽度;真机模式下与当前显示器分辨率一致
    /// @return 成功返回宽度，失败返回-1；
    static tpInt32 wmResolutionWidth();
    /// @brief 获取TpWM的分辨率的宽度;真机模式下与当前显示器分辨率一致
    /// @return 成功返回宽度，失败返回-1；
    static tpInt32 wmResolutionHeight();
    /// @brief 获取TpWM的分辨率的宽度;真机模式下与当前显示器分辨率一致
    /// @return 分辨率
    static TpSize wmResolutionSize();
    
    /// @brief 设置分辨率(只能设置支持的分辨率，否则会失败)
    /// @param width 像素宽度
    /// @param height 像素高度
    /// @return 成功返回0，失败返回-1
    tpInt32 setResolution(tpUInt32 width, tpUInt32 height);

    /// @brief 设置亮度(暂不支持)
    /// @param light
    /// @return
    tpInt32 setLight(tpUInt8 light);

    /// @brief 获取屏幕亮度(暂不支持)
    /// @return
    tpInt32 getLight();

private:
    friend class TpDisplayManage;

private:
    ITpDisplayInfoData *data_;
};

#endif
