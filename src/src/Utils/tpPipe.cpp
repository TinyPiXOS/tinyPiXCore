#include "tpPipe.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <system_error>
#include <cerrno>

tpPipe::tpPipe(const std::string &pipePath, Mode mode, bool createPipe)
    : pipePath_(pipePath), fd_(-1)
{
    if (createPipe)
    {
        if (mkfifo(pipePath.c_str(), 0666) == -1 && errno != EEXIST)
        {
            throw std::system_error(errno, std::generic_category(), "mkfifo failed");
        }
    }

    int flags = (mode == Mode::Write) ? O_WRONLY : O_RDONLY;
    flags |= O_NONBLOCK; // 添加非阻塞标志
    if ((fd_ = open(pipePath.c_str(), flags)) == -1)
    {
        throw std::system_error(errno, std::generic_category(), "open failed");
    }

    // 在构造函数末尾添加
    // if (fd_ != -1)
    // {
    //     int flags = fcntl(fd_, F_GETFL);
    //     fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK);
    // }
}

tpPipe::~tpPipe()
{
    if (fd_ != -1)
    {
        close(fd_);
    }
}

void tpPipe::send(const std::string &topic, const char *data, size_t dataLength)
{
    uint32_t topicLen = static_cast<uint32_t>(topic.size());
    uint32_t dataLen = static_cast<uint32_t>(dataLength);

    writeFull(&topicLen, sizeof(topicLen));
    writeFull(topic.data(), topicLen);
    writeFull(&dataLen, sizeof(dataLen));
    writeFull(data, dataLen);
}

tpPipe::PipeData tpPipe::recv()
{
    tpPipe::PipeData readData;

    try
    {
        uint32_t topicLen;
        readFull(&topicLen, sizeof(topicLen));

        readData.topic.resize(topicLen);
        readFull(&readData.topic[0], topicLen);

        uint32_t dataLen;
        readFull(&dataLen, sizeof(dataLen));

        readData.data.resize(dataLen);
        readFull(readData.data.data(), dataLen);
    }
    catch (const std::exception &)
    {
    }

    return readData;
}

void tpPipe::writeFull(const void *buf, size_t count)
{
    const char *p = static_cast<const char *>(buf);
    while (count > 0)
    {
        ssize_t n = write(fd_, p, count);
        if (n == -1)
        {
            throw std::system_error(errno, std::generic_category(), "write failed");
        }
        p += n;
        count -= n;
    }
}

void tpPipe::readFull(void *buf, size_t count)
{
    char *p = static_cast<char *>(buf);
    while (count > 0)
    {
        ssize_t n = read(fd_, p, count);
        if (n <= 0)
        {
            throw std::system_error(errno, std::generic_category(), "read failed");
        }
        p += n;
        count -= n;
    }
}