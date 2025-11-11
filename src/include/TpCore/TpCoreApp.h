#ifndef __TP_CORE_APP_H
#define __TP_CORE_APP_H

#include <TpCore.h>
#include <functional>

TP_DEF_VOID_TYPE_VAR(ITpCoreAppData);

class TpCoreApp
{
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

public:
    /// @brief 队列类型信号槽处理；用户无需调用
    void postEvent(std::function<void()> task);

protected:
    ITpCoreAppData *data_;
};

#endif
