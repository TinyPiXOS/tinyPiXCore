
#ifndef __TP_VAPP_H
#define __TP_VAPP_H

#include <TpUtils.h>
#include <string>
#include <functional>
#include "TpImage.h"

TP_DEF_VOID_TYPE_VAR(ITpAppData);

class TpObject;
class TpClipboard;
class TpChildWidget;
class TpScreen;
// class TpMessage;
class TpCssParser;

class TpApp
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

    /**
     * @brief 对象请求操作类型枚举
     * 该枚举定义了对象通过消息系统请求执行的操作类型，
     * 用于对象间通信和系统级操作控制。
     */
    enum
    {
        /// @brief 注册操作
        /// @details 请求将对象注册到系统顶层对象管理中，
        ///          通常用于新创建的对象需要加入系统时
        TP_REGISTER_ACT,
        /// @brief 删除操作
        /// @details 请求删除指定对象（除VScreen外），
        ///          系统将释放对象资源并移除其所有引用
        TP_DELETE_ACT,
        /// @brief 中止操作
        /// @details 请求中止当前应用程序，
        ///          系统将执行清理工作并退出程序
        TP_ABORT_ACT,
        /// @brief 返回操作
        /// @details 请求隐藏VScreen（虚拟屏幕），
        ///          通常用于界面切换或返回上级界面
        TP_RETURN_ACT,
        /// @brief 激活操作
        /// @details 请求激活指定对象，
        ///          使其成为当前焦点或可交互状态
        TP_ACTIVE_ACT,
    };

public:
    TpApp(int32_t argc, char *argv[]);
    virtual ~TpApp();

public:
    /// @brief 获取tpApp全局单例指针
    /// @return 指针对象
    static TpApp *Inst();

public:
    /// @brief 绑定应用主窗体
    /// @param object 主窗体对象指针
    /// @return 绑定结果
    virtual bool bindVScreen(TpScreen *object);
    /// @brief 开启tpApp主事件循环
    /// @return 启动结果
    virtual bool run();

public:
    virtual TpClipboard *clipboard();

    /// @brief 获取当前程序主窗体
    /// @return 主窗体指针
    virtual TpScreen *vScreen();

    /// @brief 获取全局单例CSS解析器
    /// @return css解析器智能指针
    tpShared<TpCssParser> cssParser();

    /// @brief 设置系统主题配色,设置后会更新所有UI的样式，高频率调用会造成卡顿
    /// @param style 主题值
    void setStyle(const Tp::SystemTheme &style);

    /// @brief 获取系统主题类型
    /// @return 系统主观类型
    Tp::SystemTheme style();

    /// @brief 获取当前应用界面抓图;暂未实现
    /// @return 图片资源对象
    TpImage grabWindow();

    /// @brief 唤醒虚拟键盘
    /// @return object 唤醒对象；虚拟键盘的输入将会给入该对象
    void wakeUpVirtualKeyboard(TpChildWidget *object);

    /// @brief 休眠虚拟键盘
    void dormantVirtualKeyboard();

    /// @brief 获取当前线程是否是主线程
    /// @return 主线程返回true，否则返回false
    bool isMainThread();

public:
    virtual bool isExistObject(TpObject *object, bool autoRemove = false);

public:
    virtual bool sendRegister(TpObject *object);
    virtual bool sendDelete(TpObject *object);
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
    /// @brief 队列类型信号槽处理；用户无需调用
    void postEvent(std::function<void()> task);

    /// @brief 提交刷新时间异步处理；用户无需调用
    /// @param updateObj 刷新对象指针
    /// @param x 刷新区域X
    /// @param y 刷新区域Y
    /// @param w 刷新区域W
    /// @param h 刷新区域H
    /// @param onlyBlit
    void postUpdateEvent(TpChildWidget *updateObj, const int32_t &x, const int32_t &y, const int32_t &w, const int32_t &h, bool onlyBlit);

private:
    ITpAppData *data_;
};

#endif
