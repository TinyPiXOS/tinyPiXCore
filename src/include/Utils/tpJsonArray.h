#ifndef TP_JSON_ARRAY
#define TP_JSON_ARRAY

#include <tpJsonValue.h>

/// @brief JSON数组类，封装了rapidjson数组操作
class tpJsonArray
{
public:
    rapidjson::Document doc_;

    /// @brief 默认构造函数，创建空JSON数组
    tpJsonArray();

    /// @brief 拷贝构造函数，深度复制JSON数组
    /// @param array 要复制的JSON数组
    tpJsonArray(const tpJsonArray &array);

    /// @brief 获取数组中元素的数量
    /// @return 数组元素数量
    uint32_t count();

    /// @brief 检查数组是否为空
    /// @return 空返回true，否则返回false
    bool isEmpty() const;

    /// @brief 获取指定索引位置的元素
    /// @param index 要获取的数组索引
    /// @return 索引位置对应的JSON值
    tpJsonValue at(const uint32_t &index);

    /// @brief 向数组末尾添加元素
    /// @param value 要添加的JSON值
    void append(const tpJsonValue &value);

    /// @brief 获取数组的第一个元素
    /// @return 数组的第一个元素
    tpJsonValue first();

    /// @brief 获取数组的最后一个元素
    /// @return 数组的最后一个元素
    tpJsonValue last();

    /// @brief 赋值操作符，深度复制JSON数组
    /// @param others 要复制的JSON数组
    /// @return 当前JSON数组的引用
    tpJsonArray &operator=(const tpJsonArray &others);
};

#endif