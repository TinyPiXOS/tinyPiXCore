#ifndef __TP_CORE_APP_H
#define __TP_CORE_APP_H

#include <TpCore.h>
#include <functional>

class TpObject;

TP_DEF_VOID_TYPE_VAR(ITpCoreAppData);

class TpCoreApp
{
public:
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
    /// @brief 主事件循环构造函数
    /// @param argc 参数数量
    /// @param argv 入口参数
    /// @param deskTopStrKey 桌面唯一标识；普通应用无需处理
    TpCoreApp(int32_t argc, char *argv[]);
    virtual ~TpCoreApp();

    /// @brief 获取主函数创建的 TpCoreApp 全局单例指针
    /// @return 指针对象
    static TpCoreApp *Inst();

public:
    /// @brief 开启主事件循环
    /// @return 启动结果
    virtual bool run();

    /// @brief 获取当前线程是否是主线程
    /// @return 主线程返回true，否则返回false
    bool isMainThread();

    virtual bool isExistObject(TpObject *object, bool autoRemove = false);

    virtual bool sendRegister(TpObject *object);
    virtual bool sendDelete(TpObject *object);

public:
    /// @brief 队列类型信号槽处理；用户无需调用
    void postEvent(std::function<void()> task);

protected:
    ITpCoreAppData *data_;
};

#endif
