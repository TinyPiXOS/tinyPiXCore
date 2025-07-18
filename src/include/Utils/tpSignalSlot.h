#ifndef __TP_SIGNAL_SLOT_H
#define __TP_SIGNAL_SLOT_H

#include <iostream>
#include <stdlib.h>
#include <functional>
#include <list>
#include <mutex>
#include <algorithm>
#include <tuple>
#include "tpGlobal.h"
#include "SingleGUI/core/tpApp.h"

#ifndef signals
#define signals
#endif

#ifndef slots
#define slots
#endif

template <typename... _ArgTypes>
class tpSlotBase
{
public:
	virtual void exec(_ArgTypes... args) = 0;
};

template <class T, typename... _ArgTypes>
class tpSlot : public tpSlotBase<_ArgTypes...>
{
	typedef void (T::*FuncPtr)(_ArgTypes...);

public:
	tpSlot(T *obj, FuncPtr func)
	{
		tpReceiver = obj;
		tpFunction = func;
	}

	void exec(_ArgTypes... args)
	{
		(tpReceiver->*tpFunction)(args...);
	}

	T *receiver() const
	{
		return tpReceiver;
	}

	FuncPtr function() const
	{
		return tpFunction;
	}

private:
	T *tpReceiver;
	FuncPtr tpFunction;
};

template <typename... _ArgTypes>
class tpLamdaSlot : public tpSlotBase<_ArgTypes...>
{
	typedef std::function<void(_ArgTypes...)> FuncPtr;

public:
	tpLamdaSlot(FuncPtr _func)
	{
		tpFunction_ = _func;
	}

	void exec(_ArgTypes... args)
	{
		tpFunction_(args...);
	}

	FuncPtr function()
	{
		return tpFunction_;
	}

private:
	FuncPtr tpFunction_;
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
class tpSignal
{
private:
	struct Connection
	{
		tinyPiX::ConnectionType type;
		tpSlotBase<_ArgTypes...> *slot;
		LambdaConnectionManager::ConnectionID lambdaID = 0;
	};

public:
	~tpSignal()
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
		do_connect(obj, func, tinyPiX::AutoConnection);
	}

	template <class T>
	void connect(T *obj, void (T::*func)(_ArgTypes...), tinyPiX::ConnectionType type)
	{
		do_connect(obj, func, type);
	}

	// 通用lambda连接
	LambdaConnectionManager::ConnectionID connect(typename std::function<void(_ArgTypes...)> func)
	{
		return do_connect(func, tinyPiX::AutoConnection);
	}

	LambdaConnectionManager::ConnectionID connect(typename std::function<void(_ArgTypes...)> func, tinyPiX::ConnectionType type)
	{
		return do_connect(func, type);
	}

	template <typename Func>
	typename std::enable_if<
		!is_member_function_pointer<decltype(&Func::operator())>::value,
		LambdaConnectionManager::ConnectionID>::type
		connect(Func func)
	{
		return do_connect(std::function<void(_ArgTypes...)>(func), tinyPiX::AutoConnection);
	}

	template <typename Func>
	typename std::enable_if<
		!is_member_function_pointer<decltype(&Func::operator())>::value,
		LambdaConnectionManager::ConnectionID>::type
		connect(Func func, tinyPiX::ConnectionType type)
	{
		return do_connect(std::function<void(_ArgTypes...)>(func), type);
	}

	template <class T>
	void disconnect(T *obj, void (T::*func)(_ArgTypes...))
	{
		std::lock_guard<std::mutex> lock(gMutex_);
		auto it = connections_.begin();
		while (it != connections_.end())
		{
			auto *slot = dynamic_cast<tpSlot<T, _ArgTypes...> *>(it->slot);
			if (slot && slot->receiver() == obj && slot->function() == func)
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
		// 复制当前连接以避免死锁
		std::list<Connection> current_connections;
		{
			std::lock_guard<std::mutex> lock(gMutex_);
			current_connections = connections_;
		}

		for (const auto &conn : current_connections)
		{
			if (conn.type == tinyPiX::AutoConnection)
			{
				if (tpApp::Inst()->isMainThread())
				{
					conn.slot->exec(args...);
				}
				else
				{
					std::shared_ptr<tpSlotBase<_ArgTypes...>> slotRef(
						conn.slot,
						[](tpSlotBase<_ArgTypes...> *) {});

					// 使用std::bind创建任务
					auto task = std::bind(
						[](std::shared_ptr<tpSlotBase<_ArgTypes...>> slot, _ArgTypes... args)
						{
							slot->exec(args...);
						},
						slotRef,
						args...);

					// 提交到事件循环
					tpApp::Inst()->postEvent(task);
				}
			}
			else if (conn.type == tinyPiX::DirectConnection)
			{
				conn.slot->exec(args...);
			}
			else if (conn.type == tinyPiX::QueuedConnection)
			{
				// 队列连接 - 提交到事件循环
				std::shared_ptr<tpSlotBase<_ArgTypes...>> slotRef(
					conn.slot,
					[](tpSlotBase<_ArgTypes...> *) {});

				// 使用std::bind创建任务
				auto task = std::bind(
					[](std::shared_ptr<tpSlotBase<_ArgTypes...>> slot, _ArgTypes... args)
					{
						slot->exec(args...);
					},
					slotRef,
					args...);

				// 提交到事件循环
				tpApp::Inst()->postEvent(task);
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
	void do_connect(T *obj, void (T::*func)(_ArgTypes...), tinyPiX::ConnectionType type)
	{
		std::lock_guard<std::mutex> lock(gMutex_);

		// 是否已存在相同连接
		for (const auto &conn : connections_)
		{
			auto *slot = dynamic_cast<tpSlot<T, _ArgTypes...> *>(conn.slot);
			if (slot && slot->receiver() == obj && slot->function() == func)
			{
				return;
			}
		}

		// 创建新连接对象
		Connection newConn;
		newConn.type = type;
		newConn.slot = new tpSlot<T, _ArgTypes...>(obj, func);
		newConn.lambdaID = 0;

		connections_.emplace_back(newConn);
	}

	LambdaConnectionManager::ConnectionID do_connect(typename std::function<void(_ArgTypes...)> func, tinyPiX::ConnectionType type)
	{
		std::lock_guard<std::mutex> lock(gMutex_);

		auto id = LambdaConnectionManager::nextID();

		// 创建新连接对象
		Connection newConn;
		newConn.type = type;
		newConn.slot = new tpLamdaSlot<_ArgTypes...>(func);
		newConn.lambdaID = id;

		// 添加到连接列表
		connections_.emplace_back(newConn);

		return id;
	}

private:
	std::mutex gMutex_;
	std::list<Connection> connections_;
};

#define declare_signal(signal, ...) tpSignal<__VA_ARGS__> signal

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
