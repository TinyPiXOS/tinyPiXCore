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

struct TpShareMemoryData
{
    TpString name;    // 共享内存名称（包含前缀避免冲突）
    uint64_t size;    // 共享内存总大小（包含头部）
    bool isCreator;   // 标识是否为创建者
    int shmFd;        // 共享内存文件描述符
    void *mappedAddr; // 映射的内存地址指针
};

/**
 * @brief 共享内存头部元数据结构
 * 定义共享内存头部的元数据布局，用于版本控制和数据管理
 */
struct SharedHeader
{
    std::atomic<uint64_t> version; // 数据版本号（原子操作保证线程安全）
    uint32_t dataSize;             // 实际数据区域大小
    uint64_t timeStamp;            // 最后修改时间戳
    char reserved[44];             // 保留字段，填充至64字节对齐
};

/// 共享内存头部固定大小（64字节）
static const uint64_t HEADER_SIZE = sizeof(SharedHeader);

bool initializeSharedMemory(TpShareMemoryData *sharedData)
{
    // 创建或打开共享内存对象
    if (sharedData->isCreator)
    {
        // 创建者：创建新的共享内存（存在则先删除）
        sharedData->shmFd = shm_open(sharedData->name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
        if (sharedData->shmFd == -1 && errno == EEXIST)
        {
            shm_unlink(sharedData->name.c_str());
            sharedData->shmFd = shm_open(sharedData->name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
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
        sharedData->shmFd = shm_open(sharedData->name.c_str(), O_RDWR, 0666);
    }

    if (sharedData->shmFd == -1)
    {
        return false;
    }

    // 创建者需要设置共享内存大小
    if (sharedData->isCreator)
    {
        if (ftruncate(sharedData->shmFd, sharedData->size) == -1)
        {
            return false;
        }
    }

    // 映射共享内存到进程地址空间
    sharedData->mappedAddr = mmap(nullptr, sharedData->size, PROT_READ | PROT_WRITE,
                                  MAP_SHARED, sharedData->shmFd, 0);
    if (sharedData->mappedAddr == MAP_FAILED)
    {
        return false;
    }

    // 创建者需要初始化头部信息
    if (sharedData->isCreator)
    {
        SharedHeader *header = static_cast<SharedHeader *>(sharedData->mappedAddr);
        header->version.store(0, std::memory_order_relaxed);
        header->dataSize = sharedData->size - HEADER_SIZE;
        header->timeStamp = time(nullptr);
        std::memset(header->reserved, 0, sizeof(header->reserved));

        // 初始化数据区域为0
        std::memset(static_cast<char *>(sharedData->mappedAddr) + HEADER_SIZE, 0,
                    sharedData->size - HEADER_SIZE);
    }

    return true;
}

void cleanup(TpShareMemoryData *sharedData)
{
    // 解除内存映射
    if (sharedData->mappedAddr != MAP_FAILED)
    {
        munmap(sharedData->mappedAddr, sharedData->size);
        sharedData->mappedAddr = MAP_FAILED;
    }

    // 关闭文件描述符
    if (sharedData->shmFd != -1)
    {
        close(sharedData->shmFd);
        sharedData->shmFd = -1;
    }

    // 创建者需要删除共享内存对象
    if (sharedData->isCreator)
    {
        shm_unlink(sharedData->name.c_str());
    }
}

TpShareMemory::TpShareMemory(const TpString &name, uint64_t size, bool isCreator)
{
    TpShareMemoryData *sharedData = new TpShareMemoryData();
    data_ = sharedData;

    sharedData->name = "/tmp_" + name;     // 添加前缀避免命名冲突
    sharedData->size = size + HEADER_SIZE; // 总大小 = 数据大小 + 头部大小
    sharedData->isCreator = isCreator;
    sharedData->shmFd = -1;
    sharedData->mappedAddr = MAP_FAILED;

    // 初始化共享内存
    if (!initializeSharedMemory(sharedData))
    {
        cleanup(sharedData);
        throw std::runtime_error("初始化共享内存失败: " + sharedData->name);
    }
}

TpShareMemory::~TpShareMemory()
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    cleanup(sharedData);

    if (sharedData)
    {
        delete sharedData;
        data_ = nullptr;
    }
}

bool TpShareMemory::writeData(const void *data, uint64_t size, uint64_t offset)
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);

    if (!isMapped() || data == nullptr)
    {
        return false;
    }

    // 检查写入范围有效性
    if (offset + size > (sharedData->size - HEADER_SIZE))
    {
        return false;
    }

    // 计算数据区实际偏移（跳过头部）
    char *dataPtr = static_cast<char *>(sharedData->mappedAddr) + HEADER_SIZE + offset;

    // 写入数据
    memcpy(dataPtr, data, size);

    // 更新版本号和时间戳
    SharedHeader *header = static_cast<SharedHeader *>(sharedData->mappedAddr);
    incrementVersion();
    header->timeStamp = time(nullptr);

    // 确保数据写入物理内存
    if (msync(sharedData->mappedAddr, sharedData->size, MS_SYNC) == -1)
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
    if (offset + size > (sharedData->size - HEADER_SIZE))
    {
        return false;
    }

    // 计算数据区实际偏移（跳过头部）
    const char *dataPtr = static_cast<const char *>(sharedData->mappedAddr) + HEADER_SIZE + offset;

    // 读取数据
    memcpy(buffer, dataPtr, size);

    return true;
}

void *TpShareMemory::dataPtr() const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    if (sharedData->mappedAddr == MAP_FAILED)
    {
        return nullptr;
    }
    return static_cast<char *>(sharedData->mappedAddr) + HEADER_SIZE;
}

uint64_t TpShareMemory::size() const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    return sharedData->size - HEADER_SIZE; // 返回数据区大小
}

bool TpShareMemory::isMapped() const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    return sharedData->mappedAddr != MAP_FAILED;
}

const TpString &TpShareMemory::name() const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    return sharedData->name;
}

uint32_t TpShareMemory::version() const
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    if (!isMapped())
    {
        return 0;
    }
    const SharedHeader *header = static_cast<const SharedHeader *>(sharedData->mappedAddr);
    return header->version.load(std::memory_order_acquire);
}

void TpShareMemory::incrementVersion()
{
    TpShareMemoryData *sharedData = static_cast<TpShareMemoryData *>(data_);
    if (!isMapped())
    {
        return;
    }
    SharedHeader *header = static_cast<SharedHeader *>(sharedData->mappedAddr);
    header->version.fetch_add(1, std::memory_order_release);
}
