#ifndef __TP_OBJECT_H
#define __TP_OBJECT_H

#include <TpCore.h>
#include <TpString.h>
#include <TpList.h>
#include <TpHash.h>
#include <TpProperty.h>

class TpVariant;
class TpEvent;

TP_DEF_VOID_TYPE_VAR(ITpObjectData);
/// @brief TinyPiX基 类，所有组件类、工具类等均继承于本类
class TpObject
{
public:
    TpObject(TpObject *parent = nullptr);
    virtual ~TpObject();

public:
    /// @brief 设置属性值
    /// @param _name 属性名称
    /// @param _value 属性值
    virtual void setProperty(const TpString &_name, const TpVariant &_value);
    /// @brief 指定名称获取属性
    /// @param _name 属性名称
    /// @return 属性值，未查询到结果返回空对象
    TpVariant property(const TpString &_name);

    /// @brief 安装事件过滤器，安装后触发任意事件先进入filterObj对象的eventFilter
    /// @param filterObj 过滤器对象
    virtual void installEventFilter(TpObject *filterObj);
    /// @brief 卸载事件过滤器
    virtual void uninstallEventFilter();
    /// @brief 获取当前对象已经安装的事件过滤器
    /// @return 事件过滤器指针，未设置则返回空nullptr
    TpObject *eventFilterObject();

    /// @brief 事件过滤器处理函数，对象事件会先进入事件过滤器对象的本函数
    /// @param watched 触发事件的对象指针
    /// @param event 事件指针
    /// @return 如果返回true则不再触发watched对象本身的事件回调，返回false则本函数执行完毕后会执行watched对象的事件回调
    virtual bool eventFilter(TpObject *watched, TpEvent *event) { return false; };

public:
    virtual Tp::TpObjectType objectType() { return Tp::TP_UNKOWN_OBJECT; };
    virtual int32_t objectID();
    virtual bool objectActive() { return false; };

public:
    virtual void setParent(TpObject *parent);
    virtual TpObject *parent();

public:
    virtual TpObject *topObject();
    virtual TpList<TpObject *> &objectList();

public:
    virtual TpObject *find(int32_t id);

    /// @brief 删除自身指针，不会立即释放，会在下一个事件循环删除
    virtual void deleteLater();

public:
    void *operator new(size_t size);
    void *operator new[](size_t size);
    void operator delete(void *ptr);
    void operator delete[](void *ptr);

public:
    // protected:
    /// @brief 获取object类内部数据
    /// @return 数据指针
    virtual ITpObjectData *objectSets();

protected:
    void registerProperty(const TpString &name, const TpPropertyReadCallFunc &readFunc, const TpPropertyWriteCallFunc &writeFunc)
    {
        m_properties[name].first = readFunc;
        m_properties[name].second = writeFunc;
    }

private:
    void addConnection(void *signal, std::function<void()> disconnector);

    template <typename... _ArgTypes>
    friend class TpSignal;

protected:
    ITpObjectData *data_;
    // <READ, WRITE>
    TpHash<TpString, std::pair<TpPropertyReadCallFunc, TpPropertyWriteCallFunc>> m_properties;
};

#endif
