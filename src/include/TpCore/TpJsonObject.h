#ifndef TP_JSON_OBJECT
#define TP_JSON_OBJECT

#include <TpMap.h>
#include <TpJsonValue.h>
#include <TpString.h>
#include <TpList.h>

class TpJsonArray;

/// @brief JSON对象类，提供完整的JSON对象操作功能
class TpJsonObject
{
public:
    rapidjson::Document doc_; 

    /// @brief 默认构造函数，创建空JSON对象
    TpJsonObject();

    /// @brief 拷贝构造函数
    /// @param others 要复制的源JSON对象
    TpJsonObject(const TpJsonObject &others);

    /// @brief 获取JSON对象中所有键的列表
    /// @return 键列表
    TpList<TpString> keys() const;

    /// @brief 检查JSON对象是否为空
    /// @return 空对象返回true，否则返回false
    bool isEmpty() const;

    /// @brief 检查JSON对象是否包含指定键
    /// @param key 要检查的键名
    /// @return 存在返回true，否则返回false
    bool contains(const TpString &key) const;

    /// @brief 插入JSON键值对（值类型为JsonValue）
    /// @param key 键名字符串
    /// @param value JSON值对象
    /// @note 如果key已存在，则覆盖原有值
    void insert(const TpString &key, const TpJsonValue &value);

    /// @brief 插入JSON键值对（值类型为JsonObject）
    /// @param key 键名字符串
    /// @param value JSON对象
    /// @note 如果key已存在，则覆盖原有值
    void insert(const TpString &key, const TpJsonObject &value);

    /// @brief 插入JSON键值对（值类型为JsonArray）
    /// @param key 键名字符串
    /// @param value JSON数组
    /// @note 如果key已存在，则覆盖原有值
    void insert(const TpString &key, const TpJsonArray &value);

    /// @brief 获取指定键对应的JSON值
    /// @param key 键名字符串
    /// @return 对应的JSON值
    /// @note 如果键不存在，返回无效的JsonValue
    TpJsonValue value(const TpString &key) const;

    /// @brief 移除JSON对象中的指定键及其值
    /// @param key 要移除的键名
    void remove(const TpString &key);

    /// @brief 赋值操作符
    /// @param others 要复制的源JSON对象
    /// @return 当前JSON对象的引用
    TpJsonObject &operator=(const TpJsonObject &others);
};

#endif