#ifndef __TP_SIGNAL_SLOT_H
#define __TP_SIGNAL_SLOT_H

#include <iostream>
#include <stdlib.h>
#include <functional>
#include <vector>
#include <mutex>
#include <algorithm>
#include <tuple>
#include "TpGlobal.h"
#include "TpGUI/Core/TpApp.h"
#include "TpGUI/Core/TpObject.h"

#ifndef signals
#define signals
#endif

#ifndef slots
#define slots
#endif

template <typename... _ArgTypes>
class TpSlotBase
{
public:
    virtual void exec(_ArgTypes... args) = 0;
};

template <class T, typename... _ArgTypes>
class TpSlot : public TpSlotBase<_ArgTypes...>
{
    typedef void (T::*FuncPtr)(_ArgTypes...);

public:
    TpSlot(T *obj, FuncPtr func)
    {
        TpReceiver = obj;
        TpFunction = func;
    }

    void exec(_ArgTypes... args)
    {
        (TpReceiver->*TpFunction)(args...);
    }

    T *receiver() const
    {
        return TpReceiver;
    }

    FuncPtr function() const
    {
        return TpFunction;
    }

private:
    T *TpReceiver;
    FuncPtr TpFunction;
};

template <typename... _ArgTypes>
class TpLamdaSlot : public TpSlotBase<_ArgTypes...>
{
    typedef std::function<void(_ArgTypes...)> FuncPtr;

public:
    TpLamdaSlot(FuncPtr _func)
    {
        TpFunction_ = _func;
    }

    void exec(_ArgTypes... args)
    {
        TpFunction_(args...);
    }

    FuncPtr function()
    {
        return TpFunction_;
    }

private:
    FuncPtr TpFunction_;
};

// 类型萃取：判断是否为成员函数指针
template <typename>
struct is_member_function_pointer : std::false_type
{
};

template <typename T, typename U>
struct is_member_function_pointer<T U::*> : std::integral_constant<bool, std::is_member_function_pointer<T U::*>::value>
{
};

class LambdaConnectionManager
{
public:
    using ConnectionID = uint64_t;

    static ConnectionID nextID()
    {
        static std::atomic<ConnectionID> id(0);
        return ++id;
    }
};

template <typename... _ArgTypes>
class TpSignal
{
private:
    struct Connection
    {
        Tp::ConnectionType type;
        TpSlotBase<_ArgTypes...> *slot;
        LambdaConnectionManager::ConnectionID lambdaID = 0;
        std::atomic<bool> valid{true};

        Connection()
        {
        }
        Connection(const Connection &others)
        {
            this->type = others.type;
            this->slot = others.slot;
            this->lambdaID = others.lambdaID;
            this->valid.store(others.valid.load());
        }

        Connection operator=(const Connection &others)
        {
            this->type = others.type;
            this->slot = others.slot;
            this->lambdaID = others.lambdaID;
            this->valid.store(others.valid.load());
            return *this;
        }
    };

public:
    ~TpSignal()
    {
        std::lock_guard<std::mutex> lock(gMutex_);
        for (auto &conn : connections_)
        {
            delete conn.slot;
        }
        connections_.clear();
    }

    // 成员函数连接
    template <class T>
    void connect(T *obj, void (T::*func)(_ArgTypes...))
    {
        doConnect(obj, func, Tp::AutoConnection);
    }

    template <class T>
    void connect(T *obj, void (T::*func)(_ArgTypes...), Tp::ConnectionType type)
    {
        doConnect(obj, func, type);
    }

    // 通用lambda连接
    LambdaConnectionManager::ConnectionID connect(typename std::function<void(_ArgTypes...)> func)
    {
        return doConnect(func, Tp::AutoConnection);
    }

    LambdaConnectionManager::ConnectionID connect(typename std::function<void(_ArgTypes...)> func, Tp::ConnectionType type)
    {
        return doConnect(func, type);
    }

    template <typename Func>
    typename std::enable_if<
        !is_member_function_pointer<decltype(&Func::operator())>::value,
        LambdaConnectionManager::ConnectionID>::type
        connect(Func func)
    {
        return doConnect(std::function<void(_ArgTypes...)>(func), Tp::AutoConnection);
    }

    template <typename Func>
    typename std::enable_if<
        !is_member_function_pointer<decltype(&Func::operator())>::value,
        LambdaConnectionManager::ConnectionID>::type
        connect(Func func, Tp::ConnectionType type)
    {
        return doConnect(std::function<void(_ArgTypes...)>(func), type);
    }

    template <class T>
    void disconnect(T *obj, void (T::*func)(_ArgTypes...))
    {
        std::lock_guard<std::mutex> lock(gMutex_);
        auto it = connections_.begin();
        while (it != connections_.end())
        {
            auto *slot = dynamic_cast<TpSlot<T, _ArgTypes...> *>(it->slot);
            if (slot && slot->receiver() == obj && slot->function() == func)
            {
                it->valid = false;
                delete it->slot;
                it = connections_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void disconnect(LambdaConnectionManager::ConnectionID id)
    {
        std::lock_guard<std::mutex> lock(gMutex_);
        auto it = connections_.begin();
        while (it != connections_.end())
        {
            if (it->lambdaID == id)
            {
                delete it->slot;
                it = connections_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void emit(_ArgTypes... args)
    {
        std::lock_guard<std::mutex> lock(gMutex_);

        for (const auto &conn : connections_)
        {
            // 连接已失效，跳过
            if (!conn.valid)
                continue;

            if (conn.type == Tp::AutoConnection)
            {
                if (TpApp::Inst()->isMainThread())
                {
                    conn.slot->exec(args...);
                }
                else
                {
                    std::shared_ptr<TpSlotBase<_ArgTypes...>> slotRef(
                        conn.slot,
                        [](TpSlotBase<_ArgTypes...> *) {});

                    auto task = std::bind(
                        [&](std::shared_ptr<TpSlotBase<_ArgTypes...>> slot, _ArgTypes... args)
                        {
                            if (!conn.valid.load())
                                return; // 执行前检查连接是否有效
                            slot->exec(args...);
                        },
                        slotRef,
                        args...);

                    // 提交到事件循环
                    TpApp::Inst()->postEvent(task);
                }
            }
            else if (conn.type == Tp::DirectConnection)
            {
                conn.slot->exec(args...);
            }
            else if (conn.type == Tp::QueuedConnection)
            {
                // 队列连接 - 提交到事件循环
                std::shared_ptr<TpSlotBase<_ArgTypes...>> slotRef(
                    conn.slot,
                    [](TpSlotBase<_ArgTypes...> *) {});

                auto task = std::bind(
                    [&](std::shared_ptr<TpSlotBase<_ArgTypes...>> slot, _ArgTypes... args)
                    {
                        if (!conn.valid.load())
                            return; // 执行前检查连接是否有效
                        slot->exec(args...);
                    },
                    slotRef,
                    args...);

                // 提交到事件循环
                TpApp::Inst()->postEvent(task);
            }
            else
            {
            }
        }
    }

    void operator()(_ArgTypes... args)
    {
        emit(args...);
    }

private:
    template <class T>
    void doConnect(T *obj, void (T::*func)(_ArgTypes...), Tp::ConnectionType type)
    {
        // 如果对象继承自TpConnectable，注册断开回调
        TpObject *slotObj = dynamic_cast<TpObject *>(obj);
        if (!slotObj)
        {
            std::cout << "信号槽连接失败! 接收对象非 TpObject 类型" << std::endl;
            return;
        }
        // 注册解绑函数
        auto disconnector = [this, obj, func]()
        {
            this->disconnect(obj, func);
        };
        slotObj->addConnection(this, disconnector);

        std::lock_guard<std::mutex> lock(gMutex_);

        // 是否已存在相同连接
        for (const auto &conn : connections_)
        {
            auto *slot = dynamic_cast<TpSlot<T, _ArgTypes...> *>(conn.slot);
            if (slot && slot->receiver() == obj && slot->function() == func)
            {
                return;
            }
        }

        // 创建新连接对象
        Connection newConn;
        newConn.type = type;
        newConn.slot = new TpSlot<T, _ArgTypes...>(obj, func);
        newConn.lambdaID = 0;

        connections_.emplace_back(newConn);
    }

    LambdaConnectionManager::ConnectionID doConnect(typename std::function<void(_ArgTypes...)> func, Tp::ConnectionType type)
    {
        std::lock_guard<std::mutex> lock(gMutex_);

        auto id = LambdaConnectionManager::nextID();

        // 创建新连接对象
        Connection newConn;
        newConn.type = type;
        newConn.slot = new TpLamdaSlot<_ArgTypes...>(func);
        newConn.lambdaID = id;

        // 添加到连接列表
        connections_.emplace_back(newConn);

        return id;
    }

private:
    std::mutex gMutex_;
    std::vector<Connection> connections_;
};

#define declare_signal(signal, ...) TpSignal<__VA_ARGS__> signal

#define connect_1(sender, signal, func) (sender)->signal.connect(func)
#define connect_2(sender, signal, arg1, arg2) (sender)->signal.connect(arg1, arg2)
#define connect_3(sender, signal, arg1, arg2, arg3) (sender)->signal.connect(arg1, arg2, arg3)

#define GET_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME
#define connect(...) GET_MACRO(__VA_ARGS__, connect_3, connect_2, connect_1)(__VA_ARGS__)

#define disconnect_member(sender, signal, obj, func) \
    (sender)->signal.disconnect(obj, func)
// (sender)->signal.disconnect(obj, &std::remove_reference<decltype(*(obj))>::type::func)

#define disconnect_lambda(sender, signal, id) \
    (sender)->signal.disconnect(id)

#define GET_DISCONNECT_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define disconnect(...) GET_DISCONNECT_MACRO(__VA_ARGS__, disconnect_member, disconnect_lambda)(__VA_ARGS__)

#endif
