#ifndef __TP_UUID_H
#define __TP_UUID_H

#include <TpCore.h>
#include <string>

TP_DEF_VOID_TYPE_VAR(ITpUuidData);

/**
 * @class TpUuid
 * @brief UUID (Universally Unique Identifier) 生成和表示类
 * @details 该类提供了多种版本 UUID 的生成方法（包括 V1 和 V4），
 */
class TpUuid
{
public:
    /**
     * @brief 默认构造函数
     * @details 初始化一个空的 UUID。
     */
    TpUuid();
     /**
     * @brief 拷贝构造函数
     */
    TpUuid(const TpUuid &other);
    /**
     * @brief 析构函数
     */
    ~TpUuid();

    /**
     * @brief 赋值运算符重载
     * @details 通过深拷贝另一个 TpUuid 对象来赋值给当前对象。
     * @param[in] other 赋值操作的源 TpUuid 对象
     * @return TpUuid& 返回当前对象的引用，支持链式赋值
     */
    TpUuid &operator=(const TpUuid &other);

public:
    /**
     * @brief 创建 UUID（默认版本）
     * @return TpUuid 新生成的 UUID 对象
     * @note 具体默认版本取决于底层实现（sole::uuid0）。
     */
    static TpUuid createUuid();
    /**
     * @brief 创建基于时间和主机ID的 UUID (版本1)
     * @return TpUuid 新生成的 UUID 对象
     */
    static TpUuid createUuidV1();
    /**
     * @brief 创建基于随机数的 UUID (版本4)
     * @return TpUuid 新生成的 UUID 对象
     */
    static TpUuid createUuidV4();
    /**
     * @brief 从字符串解析生成 UUID 对象
     * @param[in] text 符合 UUID 格式的字符串
     * @return TpUuid 解析后生成的 UUID 对象
     * @warning 需确保输入字符串格式正确，否则行为未定义。
     */
    static TpUuid fromString(const TpString &text);

public:
    /**
     * @brief 将 UUID 转换为标准格式字符串
     * @return TpString 代表 UUID 的字符串，格式为 "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
     */
    TpString toString();
    /**
     * @brief 将 UUID 转换为 Base62 编码字符串
     * @return TpString Base62 编码的字符串表示
     */
    TpString toBase62();
    /**
     * @brief 将 UUID 转换为更易读的格式化字符串
     * @return TpString 美化后的字符串表示
     */
    TpString toPretty();

private:
    ITpUuidData *data_; 
};

#endif