#ifndef __TP_VAPP_H
#define __TP_VAPP_H

#include <TpGUI.h>
#include <TpString.h>
#include <functional>
#include "TpImage.h"
#include "TpCoreApp.h"

class TpObject;
class TpClipboard;
class TpWidget;
class TpCssParser;

TP_DEF_VOID_TYPE_VAR(ITpAppData);
class TpApp : public TpCoreApp
{
public:
    /**
     * @brief 对象事件禁用/启用标志位枚举
     * 该枚举用于控制对象对特定输入事件的响应能力，
     * 通过位掩码组合可以同时禁用或启用多种事件类型。
     */
    enum
    {
        /// @brief 禁用所有事件
        TP_DIS_ALL = 0xffffffff,
        /// @brief 禁用键盘事件
        TP_DIS_KEYBOARD = 0x01,
        /// @brief 禁用鼠标按键事件
        TP_DIS_MOUSE = 0x02,
        /// @brief 禁用鼠标移动事件
        TP_DIS_MOTION = 0x04,
        /// @brief 禁用触摸事件
        TP_DIS_FINGER = 0x08,
        /// @brief 禁用高级手势事件
        TP_DIS_DOLLAR = 0x10,
        /// @brief 禁用基础手势事件（如缩放、旋转等）
        TP_DIS_GESTURE = 0x20,
        /// @brief 启用所有事件（无禁用），默认状态，对象响应所有类型的事件
        TP_DIS_NONE = 0,
    };

public:
    /// @brief 主事件循环构造函数
    /// @param argc 参数数量
    /// @param argv 入口参数
    /// @param deskTopStrKey 桌面唯一标识；普通应用无需处理
    TpApp(int32_t argc, char *argv[], const TpString &deskStrKey = "");
    virtual ~TpApp();

    /// @brief 获取主函数创建的 TpApp 全局单例指针
    /// @return 指针对象
    static TpApp *Inst();

public:
    /// @brief 开启tpApp主事件循环
    /// @return 启动结果
    virtual bool run() override;

    /// @brief 获取剪切板单例指针
    /// @return 剪切板指针
    virtual TpClipboard *clipboard();

    /// @brief 获取应用的主窗口；无主窗口则返回nullptr
    /// @return 应用主窗口指针
    virtual TpWidget *mainWindow();

    /// @brief 当前是否是桌面进程
    /// @return 桌面进程返回true，否则返回false
    bool isDesktop();

    /// @brief 设置主窗体显示的默认区域；设置后应用主窗口以此区域为基准显示
    /// @param rect 默认区域；不设置，则默认全屏显示
    void setClipRect(const TpRect &rect = TpRect());

    /// @brief 获取全局单例CSS解析器
    /// @return css解析器智能指针
    tpShared<TpCssParser> cssParser();

    /// @brief 设置系统主题配色,设置后会更新所有UI的样式，高频率调用会造成卡顿
    /// @param style 主题值
    void setStyle(const Tp::SystemTheme &style);

    /// @brief 获取系统主题类型
    /// @return 系统主观类型
    Tp::SystemTheme style();

    /// @brief 唤醒虚拟键盘
    /// @return object 唤醒对象；虚拟键盘的输入将会给入该对象
    void wakeUpVirtualKeyboard(TpWidget *object);

    /// @brief 休眠虚拟键盘
    void dormantVirtualKeyboard();

public:
    virtual bool sendRegister(TpObject *object) override;
    virtual bool sendAbort(TpObject *object);
    virtual bool sendReturn(TpObject *object);
    virtual bool sendActive(TpObject *object, bool actived); // only for top object type

public:
    /// @brief 设置事件禁用状态；使用TP_DIS_ALL枚举
    /// @param type 事件禁用类型
    virtual void setDisableEventType(int32_t type);
    virtual int32_t disableEventType();

    virtual ITpAppData *appObjectSet();

public:
    /// @brief 提交刷新时间异步处理；用户无需调用
    /// @param updateObj 刷新对象指针
    /// @param x 刷新区域X
    /// @param y 刷新区域Y
    /// @param w 刷新区域W
    /// @param h 刷新区域H
    /// @param onlyBlit
    void postUpdateEvent(TpWidget *updateObj, const int32_t &x, const int32_t &y, const int32_t &w, const int32_t &h, bool onlyBlit);
};

#endif
