#ifndef __TP_MD5_H
#define __TP_MD5_H

#include <TpCore.h>
#include <string>

TP_DEF_VOID_TYPE_VAR(ITpMD5Data);

/**
 * @class TpMD5
 * @brief MD5 哈希生成工具类
 * @details 提供多种长度的 MD5 哈希值生成功能，支持字符串输入和自定义输出长度。
 * @note 该类非线程安全，需在单线程环境下使用。
 */
class TpMD5
{
public:
    /**
     * @enum MD5Type
     * @brief MD5 输出长度类型
     * @details 定义不同字节长度的 MD5 哈希生成选项。
     */
    enum MD5Type
    {
        GEN_32_BYTES,  ///< 生成 32 字节 MD5 值
        GEN_64_BYTES,  ///< 生成 64 字节 MD5 值
        GEN_128_BYTES, ///< 生成 128 字节 MD5 值
        GEN_256_BYTES, ///< 生成 256 字节 MD5 值
        GEN_512_BYTES  ///< 生成 512 字节 MD5 值
    };

public:
    /**
     * @brief 默认构造函数
     * @details 初始化 MD5 计算所需的数据结构
     */
    TpMD5();

    /**
     * @brief 析构函数
     * @details 清理内部资源，释放已分配的内存
     */
    ~TpMD5();

public:
    /**
     * @brief 从字符串生成 MD5 哈希值
     * @param[in] srcString 输入字符串
     * @param[in] type MD5 输出类型，默认为 32 字节
     * @return 生成的 MD5 哈希字符串，如果失败返回空指针
     * @note 输入字符串为空时将返回空指针
     */
    virtual const char *create(const TpString &srcString, MD5Type type = GEN_32_BYTES);

    /**
     * @brief 从 C 风格字符串生成 MD5 哈希值
     * @param[in] srcString 输入字符串（以空字符结尾）
     * @param[in] type MD5 输出类型，默认为 32 字节
     * @return 生成的 MD5 哈希字符串，如果失败返回空指针
     * @warning 传入的指针必须有效且以空字符结尾
     */
    virtual const char *create(const char *srcString, MD5Type type = GEN_32_BYTES);

    /**
     * @brief 获取最后生成的 MD5 字符串
     * @return 最后生成的 MD5 哈希字符串，如果没有生成过则返回空指针
     */
    virtual const char *MD5String();

    /**
     * @brief 获取最后生成的 MD5 值的长度
     * @return MD5 字符串的字节长度，如果没有生成过则返回 0
     */
    virtual int32_t MD5Length();

    /**
     * @brief 释放内部资源
     * @details 释放 MD5 计算过程中分配的内存资源，重置内部状态
     */
    virtual void release();

public:
    /**
     * @brief 静态方法：生成 MD5 哈希值
     * @param[in] input 输入数据的指针
     * @param[in] length 输入数据的长度
     * @return 生成的 32 字节 MD5 哈希字符串
     * @note 这是最简单的 MD5 生成接口，默认生成 32 字节 MD5
     */
    static const char *getnerateMD5(const char *input, int32_t length); // default 32BYTES

private:
    ITpMD5Data *data_;
};

#endif