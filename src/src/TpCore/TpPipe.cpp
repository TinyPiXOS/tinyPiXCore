#include "TpPipe.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <system_error>
#include <cerrno>

struct TpPipeData
{
    TpString pipePath_; ///< 命名管道的文件系统路径
    int fd_;            ///< 管道的文件描述符
};

/**
 * @brief 确保写入指定数量的字节
 * @param buf 要写入的数据缓冲区指针
 * @param count 要写入的字节数
 * @throw std::system_error 写入失败或写入字节数不足时抛出
 */
void writeFull(TpPipeData *pipeData, const void *buf, size_t count)
{
    const char *p = static_cast<const char *>(buf);
    while (count > 0)
    {
        ssize_t n = write(pipeData->fd_, p, count);
        if (n == -1)
        {
            throw std::system_error(errno, std::generic_category(), "write failed");
        }
        p += n;
        count -= n;
    }
}

/**
 * @brief 确保读取指定数量的字节
 * @param buf 用于存储读取数据的缓冲区指针
 * @param count 要读取的字节数
 * @throw std::system_error 读取失败（如管道破裂）或读取字节数不足（遇到文件结束）时抛出
 */
void readFull(TpPipeData *pipeData, void *buf, size_t count)
{
    char *p = static_cast<char *>(buf);
    while (count > 0)
    {
        ssize_t n = read(pipeData->fd_, p, count);
        if (n <= 0)
        {
            throw std::system_error(errno, std::generic_category(), "read failed");
        }
        p += n;
        count -= n;
    }
}

TpPipe::TpPipe(const TpString &pipePath, Mode mode, bool isBlock)
{
    TpPipeData *pipeData = new TpPipeData();
    pipeData->pipePath_ = pipePath;
    pipeData->fd_ = -1;
    data_ = pipeData;

    if (mkfifo(pipePath.c_str(), 0666) == -1 && errno != EEXIST)
    {
        throw std::system_error(errno, std::generic_category(), "mkfifo failed");
    }

    int flags = (mode == Mode::Write) ? O_WRONLY : O_RDONLY;

    // 添加非阻塞标志
    if (!isBlock)
    {
        flags |= O_NONBLOCK;
    }

    if ((pipeData->fd_ = open(pipePath.c_str(), flags)) == -1)
    {
        throw std::system_error(errno, std::generic_category(), "open failed");
    }
}

TpPipe::~TpPipe()
{
    TpPipeData *pipeData = static_cast<TpPipeData *>(data_);
    if (pipeData)
    {
        if (pipeData->fd_ != -1)
        {
            close(pipeData->fd_);
        }

        delete pipeData;
        pipeData = nullptr;
        data_ = nullptr;
    }
}

void TpPipe::send(const TpString &topic, const char *data, uint32_t dataLength)
{
    TpPipeData *pipeData = static_cast<TpPipeData *>(data_);

    uint32_t topicLen = static_cast<uint32_t>(topic.size());
    uint32_t dataLen = dataLength;

    writeFull(pipeData, &topicLen, sizeof(topicLen));
    writeFull(pipeData, topic.data(), topicLen);
    writeFull(pipeData, &dataLen, sizeof(dataLen));
    writeFull(pipeData, data, dataLen);
}

TpPipe::PipeData TpPipe::recv()
{
    TpPipeData *pipeData = static_cast<TpPipeData *>(data_);

    TpPipe::PipeData readData;

    try
    {
        uint32_t topicLen;
        readFull(pipeData, &topicLen, sizeof(topicLen));

        readData.topic.resize(topicLen);
        readFull(pipeData, &readData.topic[0], topicLen);

        uint32_t dataLen;
        readFull(pipeData, &dataLen, sizeof(dataLen));

        readData.data.resize(dataLen);
        readFull(pipeData, readData.data.data(), dataLen);
    }
    catch (const std::exception &)
    {
    }

    return readData;
}
