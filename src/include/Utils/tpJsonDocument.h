#ifndef TP_JSON_DOCUMENT
#define TP_JSON_DOCUMENT

#include <tpString.h>
#include <tpJsonObject.h>
#include <tpJsonArray.h>

/// @brief JSON文档类，封装了完整的JSON文档操作
class tpJsonDocument
{
public:
    rapidjson::Document doc_;  ///< @brief 底层的rapidjson文档对象，存储JSON数据

    /// @brief 默认构造函数，创建空JSON文档
    tpJsonDocument();

    /// @brief 从JSON对象构造文档
    /// @param object 源JSON对象
    tpJsonDocument(const tpJsonObject& object);

    /// @brief 从JSON数组构造文档
    /// @param array 源JSON数组
    tpJsonDocument(const tpJsonArray& array);

    /// @brief 拷贝构造函数
    /// @param other 要复制的JSON文档
    tpJsonDocument(const tpJsonDocument &other);

    /// @brief 从JSON字符串解析创建文档
    /// @param json JSON格式字符串
    /// @return 解析后的JSON文档对象
    /// @note 如果解析失败将返回空文档
    static tpJsonDocument fromJson(const tpString &json);

    /// @brief 将文档序列化为JSON字符串
    /// @return JSON格式字符串
    tpString toJson();

    /// @brief 将文档作为JSON对象访问
    /// @return JSON对象
    /// @note 如果文档不是对象类型，返回值可能无效
    tpJsonObject object() const;
    
    /// @brief 将文档作为JSON数组访问
    /// @return JSON数组
    /// @note 如果文档不是数组类型，返回值可能无效
    tpJsonArray array() const;

    /// @brief 赋值操作符
    /// @param others 要复制的源文档
    /// @return 当前文档的引用
    tpJsonDocument &operator=(const tpJsonDocument &others);
};

#endif