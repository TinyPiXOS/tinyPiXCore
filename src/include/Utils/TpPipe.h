#ifndef __TP_PIPE_H
#define __TP_PIPE_H

#include "TpString.h"
#include "TpVector.h"

#include <string>
#include <vector>
#include <cstdint>

class TpPipe
{
public:
    enum class Mode
    {
        Read,
        Write
    };

    struct PipeData
    {
        TpString topic;
        TpVector<char> data;
    };

    TpPipe(const std::string &pipePath, Mode mode, bool createPipe = true);
    ~TpPipe();

    void send(const std::string &topic, const char *data, size_t dataLength);
    PipeData recv();

private:
    void writeFull(const void *buf, size_t count);
    void readFull(void *buf, size_t count);

    std::string pipePath_;
    int fd_;
};
#endif // TP_PIPE_H