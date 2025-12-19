#ifndef TPSHARE_MEMORY_H
#define TPSHARE_MEMORY_H

#include <TpString.h>
#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ITpShareMemoryData);
/**
 * @class TpShareMemory
 * @brief 跨进程共享内存管理类
 * 该类提供了共享内存的创建、映射、读写等操作
 * 支持数据版本管理和并发访问控制，确保多进程间数据同步的安全性
 */
class TpShareMemory
{
public:
    /**
     * @brief 构造函数：初始化共享内存对象
     * @param name 共享内存名称，用于唯一标识共享内存区域
     * @param size 共享内存数据区大小（不包含头部元数据）
     * @param is_creator 标识是否为创建者（true-创建新共享内存，false-打开已存在的共享内存）
     * @exception std::runtime_error 当共享内存初始化失败时抛出异常
     * @note 如果是创建者，且共享内存已存在，会先删除再重新创建
     */
    TpShareMemory(const TpString &name, uint64_t size, bool isCreator = false);

    /**
     * @brief 析构函数：自动清理共享内存资源
     * @note 如果是创建者，会自动删除共享内存对象；否则只解除映射
     */
    ~TpShareMemory();

    // 禁用拷贝构造和赋值操作
    TpShareMemory(const TpShareMemory &) = delete;            ///< 禁用拷贝构造
    TpShareMemory &operator=(const TpShareMemory &) = delete; ///< 禁用赋值操作

    /**
     * @brief 写入数据到共享内存的指定位置
     * @param data 要写入的数据指针
     * @param size 要写入的数据大小（字节）
     * @param offset 在共享内存数据区中的偏移量（默认从0开始）
     * @return bool 写入成功返回true，失败返回false
     * @note 写入成功后版本号会自动递增，时间戳更新
     */
    bool writeData(const void *data, uint64_t size, uint64_t offset = 0);

    /**
     * @brief 从共享内存读取数据到缓冲区
     * @param buffer 接收数据的缓冲区指针
     * @param size 要读取的数据大小（字节）
     * @param offset 在共享内存数据区中的偏移量（默认从0开始）
     * @return bool 读取成功返回true，失败返回false
     * @note 共享内存必须已成功映射且缓冲区指针非空
     */
    bool readData(void *buffer, uint64_t size, uint64_t offset = 0) const;

    /**
     * @brief 获取共享内存数据区的起始指针
     * @return void* 指向数据区起始地址的指针（跳过头部元数据）
     * @note 返回的指针可以直接访问数据区，但需要注意并发访问安全
     */
    void *dataPtr() const;

    /**
     * @brief 获取共享内存总大小（包含头部）
     * @return uint64_t 共享内存总大小（字节）
     */
    uint64_t size() const;

    /**
     * @brief 检查共享内存是否映射成功
     * @return bool 映射成功返回true，失败返回false
     * @retval true 共享内存已成功映射到进程地址空间
     * @retval false 共享内存映射失败
     */
    bool isMapped() const;

    /**
     * @brief 获取共享内存名称
     * @return const TpString& 共享内存名称的常量引用
     */
    const TpString &name() const;

    /**
     * @brief 获取当前数据版本号
     * @return uint32_t 当前数据版本号
     * @note 版本号用于检测数据变更，每次写入操作会自动递增
     */
    uint32_t version() const;

    /**
     * @brief 递增数据版本号
     * @note 通常由writeData自动调用，也可手动调用强制版本更新
     */
    void incrementVersion();

private:
    ITpShareMemoryData *data_;
};

#endif // TPSHARE_MEMORY_H