#include "TpShareMemory.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <system_error>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <atomic>
#include <cstring>

/**
 * @struct TpShareMemoryData
 * @brief 共享内存实现数据结构（PIMPL模式）
 *
 * 该结构体隐藏了TpShareMemory类的实现细节，包含所有必要的成员变量[8](@ref)。
 */
struct TpShareMemoryData
{
    TpString name_;     ///< 共享内存名称（包含前缀避免冲突）
    uint64_t size_;     ///< 共享内存总大小（包含头部）
    bool is_creator_;   ///< 标识是否为创建者
    int shm_fd_;        ///< 共享内存文件描述符
    void *mapped_addr_; ///< 映射的内存地址指针
};

/**
 * @struct SharedHeader
 * @brief 共享内存头部元数据结构
 *
 * 定义共享内存头部的元数据布局，用于版本控制和数据管理[8](@ref)。
 */
struct SharedHeader
{
    std::atomic<uint64_t> version; ///< 数据版本号（原子操作保证线程安全）
    uint32_t data_size;            ///< 实际数据区域大小
    uint64_t timestamp;            ///< 最后修改时间戳
    char reserved[44];             ///< 保留字段，填充至64字节对齐
};

/// 共享内存头部固定大小（64字节）
static const uint64_t HEADER_SIZE = sizeof(SharedHeader);

TpShareMemory::TpShareMemory(const TpString &name, uint64_t size, bool is_creator)
{
    // 创建PIMPL实现对象
    TpShareMemoryData *sharedData = new TpShareMemoryData();
    data_ = sharedData;

    // 初始化成员变量
    sharedData->name_ = "/tmp_" + name;     // 添加前缀避免命名冲突
    sharedData->size_ = size + HEADER_SIZE; // 总大小 = 数据大小 + 头部大小
    sharedData->is_creator_ = is_creator;
    sharedData->shm_fd_ = -1;
    sharedData->mapped_addr_ = MAP_FAILED;

    // 初始化共享内存
    if (!initializeSharedMemory())
    {
        cleanup();
        throw std::runtime_error("初始化共享内存失败: " + sharedData->name_);
    }
}

TpShareMemory::~TpShareMemory()
{
    cleanup();

    // 释放PIMPL对象
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    if (sharedData)
    {
        delete sharedData;
        data_ = nullptr;
    }
}

bool TpShareMemory::initializeSharedMemory()
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);

    // 创建或打开共享内存对象
    if (sharedData->is_creator_)
    {
        // 创建者：创建新的共享内存（存在则先删除）
        sharedData->shm_fd_ = shm_open(sharedData->name_.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
        if (sharedData->shm_fd_ == -1 && errno == EEXIST)
        {
            shm_unlink(sharedData->name_.c_str());
            sharedData->shm_fd_ = shm_open(sharedData->name_.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
        }

        // 立即取消链接，实现自动销毁
        // if (shm_unlink(sharedData->name_.c_str()) == -1)
        // {
        //     perror("shm_unlink");
        //     // 继续执行，不算致命错误
        // }
        // printf("共享内存已标记为自动销毁。\n");
    }
    else
    {
        // 使用者：打开已存在的共享内存
        sharedData->shm_fd_ = shm_open(sharedData->name_.c_str(), O_RDWR, 0666);
    }

    if (sharedData->shm_fd_ == -1)
    {
        return false;
    }

    // 创建者需要设置共享内存大小
    if (sharedData->is_creator_)
    {
        if (ftruncate(sharedData->shm_fd_, sharedData->size_) == -1)
        {
            return false;
        }
    }

    // 映射共享内存到进程地址空间
    sharedData->mapped_addr_ = mmap(nullptr, sharedData->size_, PROT_READ | PROT_WRITE,
                                    MAP_SHARED, sharedData->shm_fd_, 0);
    if (sharedData->mapped_addr_ == MAP_FAILED)
    {
        return false;
    }

    // 创建者需要初始化头部信息
    if (sharedData->is_creator_)
    {
        SharedHeader *header = static_cast<SharedHeader *>(sharedData->mapped_addr_);
        header->version.store(0, std::memory_order_relaxed);
        header->data_size = sharedData->size_ - HEADER_SIZE;
        header->timestamp = time(nullptr);
        std::memset(header->reserved, 0, sizeof(header->reserved));

        // 初始化数据区域为0
        std::memset(static_cast<char *>(sharedData->mapped_addr_) + HEADER_SIZE, 0,
                    sharedData->size_ - HEADER_SIZE);
    }

    return true;
}

bool TpShareMemory::writeData(const void *data, uint64_t size, uint64_t offset)
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);

    if (!isMapped() || data == nullptr)
    {
        return false;
    }

    // 检查写入范围有效性
    if (offset + size > (sharedData->size_ - HEADER_SIZE))
    {
        return false;
    }

    // 计算数据区实际偏移（跳过头部）
    char *data_ptr = static_cast<char *>(sharedData->mapped_addr_) + HEADER_SIZE + offset;

    // 写入数据
    memcpy(data_ptr, data, size);

    // 更新版本号和时间戳
    SharedHeader *header = static_cast<SharedHeader *>(sharedData->mapped_addr_);
    incrementVersion();
    header->timestamp = time(nullptr);

    // 确保数据写入物理内存
    if (msync(sharedData->mapped_addr_, sharedData->size_, MS_SYNC) == -1)
    {
        return false;
    }

    return true;
}

bool TpShareMemory::readData(void *buffer, uint64_t size, uint64_t offset) const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);

    if (!isMapped() || buffer == nullptr)
    {
        return false;
    }

    // 检查读取范围有效性
    if (offset + size > (sharedData->size_ - HEADER_SIZE))
    {
        return false;
    }

    // 计算数据区实际偏移（跳过头部）
    const char *data_ptr = static_cast<const char *>(sharedData->mapped_addr_) + HEADER_SIZE + offset;

    // 读取数据
    memcpy(buffer, data_ptr, size);

    return true;
}

void *TpShareMemory::getDataPtr() const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    if (sharedData->mapped_addr_ == MAP_FAILED)
    {
        return nullptr;
    }
    return static_cast<char *>(sharedData->mapped_addr_) + HEADER_SIZE;
}

uint64_t TpShareMemory::getSize() const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    return sharedData->size_ - HEADER_SIZE; // 返回数据区大小
}

bool TpShareMemory::isMapped() const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    return sharedData->mapped_addr_ != MAP_FAILED;
}

const TpString &TpShareMemory::getName() const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    return sharedData->name_;
}

uint32_t TpShareMemory::getVersion() const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    if (!isMapped())
    {
        return 0;
    }
    const SharedHeader *header = static_cast<const SharedHeader *>(sharedData->mapped_addr_);
    return header->version.load(std::memory_order_acquire);
}

void TpShareMemory::incrementVersion()
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    if (!isMapped())
    {
        return;
    }
    SharedHeader *header = static_cast<SharedHeader *>(sharedData->mapped_addr_);
    header->version.fetch_add(1, std::memory_order_release);
}

void TpShareMemory::cleanup()
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);

    // 解除内存映射
    if (sharedData->mapped_addr_ != MAP_FAILED)
    {
        munmap(sharedData->mapped_addr_, sharedData->size_);
        sharedData->mapped_addr_ = MAP_FAILED;
    }

    // 关闭文件描述符
    if (sharedData->shm_fd_ != -1)
    {
        close(sharedData->shm_fd_);
        sharedData->shm_fd_ = -1;
    }

    // 创建者需要删除共享内存对象
    if (sharedData->is_creator_)
    {
        shm_unlink(sharedData->name_.c_str());
    }
}