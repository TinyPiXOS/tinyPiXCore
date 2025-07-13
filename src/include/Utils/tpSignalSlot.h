#ifndef __TP_SIGNAL_SLOT_H
#define __TP_SIGNAL_SLOT_H

#include <iostream>
#include <stdlib.h>
#include <functional>
#include <list>
#include <mutex>
#include <algorithm>

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

	tpSlot(T *obj, FuncPtr func(_ArgTypes... args))
	{
		tpReceiver = obj;
		tpFunction = func;
	}

	void exec(_ArgTypes... args)
	{
		(tpReceiver->*tpFunction)(args...);
	}

	T *receiver()
	{
		return tpReceiver;
	}

	FuncPtr function()
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
	// typedef void *(*FuncPtr)(_ArgTypes...);

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

// can not be inherited
template <typename... _ArgTypes>
class tpSignal
{
public:
	~tpSignal()
	{
		gMutex.lock();

		for (auto iter = tpSlotBasePtr.begin(); iter != tpSlotBasePtr.end(); iter++)
		{
			delete (*iter);
		}

		tpSlotBasePtr.clear();

		gMutex.unlock();
	}

	template <class T>
	void connect(T *obj, void (T::*func)(_ArgTypes... args))
	{
		gMutex.lock();

		tpSlot<T, _ArgTypes...> *tpSB = new tpSlot<T, _ArgTypes...>(obj, func);
		auto iter = std::find_if(tpSlotBasePtr.begin(), tpSlotBasePtr.end(), [tpSB](const tpSlotBase<_ArgTypes...> *value)
								 {
			tpSlot<T,_ArgTypes...> *tmp = (tpSlot<T,_ArgTypes...>*)value;
			return ((tpSB->receiver() == tmp->receiver()) && (tpSB->function() == tmp->function())); });

		if (iter != tpSlotBasePtr.end())
		{
			delete tpSB;
		}
		else
		{
			tpSlotBasePtr.push_back(tpSB);
		}

		gMutex.unlock();
	}

	void connect(std::function<void(_ArgTypes... args)> func)
	{
		gMutex.lock();

		tpLamdaSlot<_ArgTypes...> *tpSB = new tpLamdaSlot<_ArgTypes...>(func);
		// auto iter = std::find_if(tpSlotBasePtr.begin(), tpSlotBasePtr.end(), [tpSB](const tpSlotBase<_ArgTypes...> *value)
		// 						 {
		// 	tpLamdaSlot<_ArgTypes...> *tmp = (tpLamdaSlot<_ArgTypes...>*)value;

		// 	std::function<void(_ArgTypes...)> tmpFunc = tmp->function();
		// 	std::function<void(_ArgTypes...)> tpSBFunc = tpSB->function();

		// 	// 获取两个函数的目标类型
		// 	// const void* target1 = tmpFunc.target<void(_ArgTypes...)>();
		// 	// const void* target2 = tpSBFunc.target<void(_ArgTypes...)>();

		// 	// auto tmpFuncPtr = tmpFunc.target<void(*)(_ArgTypes... args)>();

		// 	// bool eq = tmp->function().target<void(_ArgTypes...)>() == tpSB->function().target<void(_ArgTypes...)>();
		// 	bool eq = true;
		// 	return eq; });

		// if (iter != tpSlotBasePtr.end())
		// {
		// 	delete tpSB;
		// }
		// else
		// {
		// 	tpSlotBasePtr.push_back(tpSB);
		// }
		tpSlotBasePtr.push_back(tpSB);

		gMutex.unlock();
	}

	template <class T>
	void disconnect(T *obj, void (T::*func)(_ArgTypes... args))
	{
		gMutex.lock();

		tpSlot<T, _ArgTypes...> *tpSB = new tpSlot<T, _ArgTypes...>(obj, func);
		auto iter = std::find_if(tpSlotBasePtr.begin(), tpSlotBasePtr.end(), [tpSB](const tpSlotBase<_ArgTypes...> *value)
								 {
			tpSlot<T,_ArgTypes...> *tmp = (tpSlot<T,_ArgTypes...>*)value;
			return ((tpSB->receiver() == tmp->receiver()) && (tpSB->function() == tmp->function())); });

		if (iter != tpSlotBasePtr.end())
		{
			tpSlotBasePtr.erase(iter);
		}

		delete tpSB;

		gMutex.unlock();
	}

	void emit(_ArgTypes... args)
	{
		gMutex.lock();

		for (auto &slot : tpSlotBasePtr)
		{
			(*slot).exec(args...);
		}

		gMutex.unlock();
	}

	void operator()(_ArgTypes... args)
	{
		this->emit(args...);
	}

private:
	std::mutex gMutex;
	std::list<tpSlotBase<_ArgTypes...> *> tpSlotBasePtr;
};

#define declare_signal(signal, ...) tpSignal<__VA_ARGS__> signal

#define SIGNALS(type, signal, ...) signal
#define SLOTS(type, slot, ...) type::slot

#define connect_sig_slot_1(sender, signal, target, slot) ((sender)->signal.connect(target, &slot))
#define connect_sig_slot_2(sender, signal, slot) ((sender)->signal.connect(slot))

#define get_macro(_1, _2, _3, _4, NAME, ...) NAME
#define connect(...) get_macro(__VA_ARGS__, connect_sig_slot_1, connect_sig_slot_2)(__VA_ARGS__)

#define disconnect(sender, signal, target, slot) ((sender)->signal.disconnect(target, &slot))

#endif
