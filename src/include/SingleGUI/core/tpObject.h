#ifndef __TP_VOBJECT_H
#define __TP_VOBJECT_H

#include <tpUtils.h>
#include <tpString.h>
#include <tpList.h>

TP_DEF_VOID_TYPE_VAR(IPitpObject);

class tpRect;
class tpPoint;
class tpSurface;
class tpColors;

class tpKeyboardEvent;
class tpMouseEvent;
class tpFingerEvent;
class tpDollAREvent;
class tpMultiGestureEvent;

class tpObjectMoveEvent;
class tpObjectResizeEvent;
class tpObjectFocusEvent;
class tpObjectLeaveEvent;
class tpObjectVisibleEvent;
class tpObjectRotateEvent;
class tpObjectPaintEvent;
class tpObjectActiveEvent;
class tpThemeChangeEvent;

class tpObjectList;
class tpLayout;
class tpHookScreen;
class tpVariant;
class tpEvent;

/// @brief tinyPiX基类，所有组件类、工具类等均继承于本类
class tpObject
{
public:
	tpObject(tpObject *parent = nullptr);
	virtual ~tpObject();

public:
	/// @brief 设置属性值
	/// @param _name 属性名称
	/// @param _value 属性值
	virtual void setProperty(const tpString &_name, const tpVariant &_value);
	/// @brief 指定名称获取属性
	/// @param _name 属性名称
	/// @return 属性值，未查询到结果返回空对象
	tpVariant property(const tpString &_name);

	/// @brief 安装事件过滤器，安装后触发任意事件先进入filterObj对象的eventFilter
	/// @param filterObj 过滤器对象
	virtual void installEventFilter(tpObject *filterObj);
	/// @brief 卸载事件过滤器
	virtual void uninstallEventFilter();
	/// @brief 获取当前对象已经安装的事件过滤器
	/// @return 事件过滤器指针，未设置则返回空nullptr
	tpObject *eventFilterObject();

	/// @brief 事件过滤器处理函数，对象事件会先进入事件过滤器对象的本函数
	/// @param watched 触发事件的对象指针
	/// @param event 事件指针
	/// @return 如果返回true则不再触发watched对象本身的事件回调，返回false则本函数执行完毕后会执行watched对象的事件回调
	virtual bool eventFilter(tpObject *watched, tpEvent *event);

public:
	virtual ItpObjectType objectType() { return TP_UNKOWN_OBJECT; };
	virtual int32_t objectID();
	virtual bool objectActive() { return false; };

public:
	virtual void setParent(tpObject *parent);
	virtual tpObject *parent();

public:
	virtual tpObject *topObject();

public:
	virtual tpList<tpObject *> &objectList();

	// protected:
	/// @brief 获取object类内部数据
	/// @return 数据指针
	virtual IPitpObject *objectSets();

public:
	virtual void broadSetTop();
	virtual tpObject *find(int32_t id);

	/// @brief 删除自身指针，不会立即释放，会在下一个事件循环删除
	virtual void deleteLater();

public:
	void *operator new(size_t size);
	void *operator new[](size_t size);
	void operator delete(void *ptr);
	void operator delete[](void *ptr);

private:
	IPitpObject *objectSet;
};

#endif
