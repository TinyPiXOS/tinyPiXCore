#ifndef TP_JSON_OBJECT
#define TP_JSON_OBJECT

#include <tpMap.h>
#include <tpJsonValue.h>
#include <tpString.h>
#include <tpList.h>

class tpJsonArray;

/// @brief JSON对象类，提供完整的JSON对象操作功能
class tpJsonObject
{
public:
    rapidjson::Document doc_; 

    /// @brief 默认构造函数，创建空JSON对象
    tpJsonObject();

    /// @brief 拷贝构造函数
    /// @param others 要复制的源JSON对象
    tpJsonObject(const tpJsonObject &others);

    /// @brief 获取JSON对象中所有键的列表
    /// @return 键列表
    tpList<tpString> keys() const;

    /// @brief 检查JSON对象是否为空
    /// @return 空对象返回true，否则返回false
    bool isEmpty() const;

    /// @brief 检查JSON对象是否包含指定键
    /// @param key 要检查的键名
    /// @return 存在返回true，否则返回false
    bool contains(const tpString &key) const;

    /// @brief 插入JSON键值对（值类型为JsonValue）
    /// @param key 键名字符串
    /// @param value JSON值对象
    /// @note 如果key已存在，则覆盖原有值
    void insert(const tpString &key, const tpJsonValue &value);

    /// @brief 插入JSON键值对（值类型为JsonObject）
    /// @param key 键名字符串
    /// @param value JSON对象
    /// @note 如果key已存在，则覆盖原有值
    void insert(const tpString &key, const tpJsonObject &value);

    /// @brief 插入JSON键值对（值类型为JsonArray）
    /// @param key 键名字符串
    /// @param value JSON数组
    /// @note 如果key已存在，则覆盖原有值
    void insert(const tpString &key, const tpJsonArray &value);

    /// @brief 获取指定键对应的JSON值
    /// @param key 键名字符串
    /// @return 对应的JSON值
    /// @note 如果键不存在，返回无效的JsonValue
    tpJsonValue value(const tpString &key);

    /// @brief 移除JSON对象中的指定键及其值
    /// @param key 要移除的键名
    void remove(const tpString &key);

    /// @brief 赋值操作符
    /// @param others 要复制的源JSON对象
    /// @return 当前JSON对象的引用
    tpJsonObject &operator=(const tpJsonObject &others);
};

#endif