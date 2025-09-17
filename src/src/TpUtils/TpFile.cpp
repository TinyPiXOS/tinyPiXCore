#include "TpFile.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
#include <cstring>

struct TpFileData
{
    TpString fileName;

    std::fstream fileStream;
    TpFile::OpenModeFlag openMode;

    TpFileData() : fileName(""), openMode(TpFile::NotOpen)
    {
    }
};

std::ios::openmode convertOpenMode(TpFile::OpenModeFlag mode)
{
    std::ios::openmode iosMode = std::ios::in | std::ios::out;

    switch (mode)
    {
    case TpFile::ReadOnly:
        iosMode = std::ios::in;
        break;
    case TpFile::WriteOnly:
        iosMode = std::ios::out;
        break;
    case TpFile::ReadWrite:
        iosMode = std::ios::in | std::ios::out;
        break;
    case TpFile::Append:
        iosMode = std::ios::app;
        break;
    // case TpFile::Truncate:
    //     iosMode = std::ios::trunc;
    //     break;
    // case TpFile::Text:
    //     iosMode = std::ios::ate;
    //     break;
    default:
        break;
    }

    return iosMode;
}

TpFile::TpFile()
{
    this->data_ = new TpFileData();
}

TpFile::TpFile(const TpString &_fileName)
{
    this->data_ = new TpFileData();

    setFileName(_fileName);
}

TpFile::~TpFile()
{
}

bool TpFile::exists(const TpString &fileName)
{
    if (fileName.empty())
        return false;

    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

bool TpFile::rename(const TpString &oldName, const TpString &newName)
{
    if (!TpFile::exists(oldName))
        return false;

    return (std::rename(oldName.c_str(), newName.c_str()) == 0);
}

bool TpFile::copy(const TpString &fileName, const TpString &newName)
{
    if (!TpFile::exists(fileName))
        return false;

    std::ifstream src(fileName, std::ios::binary);
    std::ofstream dst(newName, std::ios::binary);

    dst << src.rdbuf();
    return dst.good();
}

bool TpFile::remove(const TpString &fileName)
{
    if (!TpFile::exists(fileName))
        return false;

    return (std::remove(fileName.c_str()) == 0);
}

TpString TpFile::fileName() const
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return "";

    return data->fileName;
}

void TpFile::setFileName(const TpString &name)
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return;

    data->fileName = name;
}

TpFileInfo TpFile::fileInfo()
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return TpFileInfo();

    TpFileInfo fileInfo(data->fileName);
    return fileInfo;
}

bool TpFile::exists() const
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data || data->fileName.empty())
        return false;

    struct stat buffer;
    return (stat(data->fileName.c_str(), &buffer) == 0);

    return false;
}

bool TpFile::remove()
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data || data->fileName.empty())
        return false;

    return (std::remove(data->fileName.c_str()) == 0);
}

bool TpFile::rename(const TpString &newName)
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data || data->fileName.empty())
        return false;

    return (std::rename(data->fileName.c_str(), newName.c_str()) == 0);
}

bool TpFile::copy(const TpString &newName)
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data || data->fileName.empty())
        return false;

    std::ifstream src(data->fileName, std::ios::binary);
    std::ofstream dst(newName, std::ios::binary);

    dst << src.rdbuf();
    return dst.good();
}

uint64_t TpFile::size() const
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return 0;

    if (data->fileName.empty() || !exists())
        return 0;

    struct stat buffer;
    if (stat(data->fileName.c_str(), &buffer) != 0)
    {
        throw std::runtime_error("Unable to get file size");
    }
    return static_cast<uint64_t>(buffer.st_size);
}

bool TpFile::open(OpenModeFlag mode)
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return false;

    if (isOpen())
        close();

    if (mode == TpFile::NotOpen)
        return false;

    data->openMode = mode;
    data->fileStream.open(data->fileName, convertOpenMode(mode));
    return data->fileStream.is_open();
}

bool TpFile::isOpen() const
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return false;

    return data->fileStream.is_open();
}

bool TpFile::isReadable() const
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return false;

    bool readAble = ((data->openMode & TpFile::ReadOnly) != 0) || ((data->openMode & TpFile::ReadWrite) != 0);

    return (data->openMode & TpFile::ReadOnly) != 0;
}

bool TpFile::isWritable() const
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return false;

    bool writeAble = ((data->openMode & TpFile::WriteOnly) != 0) || ((data->openMode & TpFile::ReadWrite) != 0) || ((data->openMode & TpFile::Append) != 0);

    return writeAble;
}

void TpFile::close()
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return;

    if (isOpen())
    {
        data->fileStream.close();
        data->openMode = TpFile::NotOpen;
    }
}

uint64_t TpFile::pos() const
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return 0;

    return static_cast<uint64_t>(data->fileStream.tellg());
}

bool TpFile::seek(uint64_t offset)
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return false;

    if (!isOpen())
        return false;

    data->fileStream.seekg(offset);
    data->fileStream.seekp(offset);
    return true;

    return false;
}

bool TpFile::atEnd() const
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return false;

    return data->fileStream.eof();
}

bool TpFile::flush()
{
    TpFileData *data = (TpFileData *)this->data_;
    if (!data)
        return false;

    if (isOpen())
    {
        data->fileStream.flush();
        return data->fileStream.good();
    }

    return false;
}

uint64_t TpFile::read(char *data, uint64_t maxlen)
{
    TpFileData *fileData = (TpFileData *)this->data_;
    if (!fileData)
        return false;

    if (!isOpen() || !isReadable())
        return 0;

    fileData->fileStream.read(data, maxlen);
    return fileData->fileStream.gcount();
}

TpString TpFile::read(uint64_t maxlen)
{
    TpFileData *fileData = (TpFileData *)this->data_;
    if (!fileData)
        return "";

    if (!isOpen() || !isReadable())
        return "";

    TpString buffer;
    buffer.resize(maxlen);
    fileData->fileStream.read(&buffer[0], maxlen);
    buffer.resize(fileData->fileStream.gcount());
    return buffer;
}

TpString TpFile::readAll()
{
    TpFileData *fileData = (TpFileData *)this->data_;
    if (!fileData)
        return "";

    if (!isOpen() || !isReadable())
        return "";

    TpString buffer;
    std::streampos pos = fileData->fileStream.tellg();
    fileData->fileStream.seekg(0, std::ios::end);
    buffer.resize(fileData->fileStream.tellg() - pos);
    fileData->fileStream.seekg(pos);
    fileData->fileStream.read(&buffer[0], buffer.size());
    return buffer;
}

uint64_t TpFile::readLine(char *data, uint64_t maxlen)
{
    /*
        std::fstream的getline方法会在读取的字符串末尾自动添加一个空字符，
        所以在使用readLine(char *data, uint64_t maxlen)时，
        maxlen应该至少比要读取的行多一个字符，以容纳空字符
    */
    TpFileData *fileData = (TpFileData *)this->data_;
    if (!fileData)
        return 0;

    if (!isOpen() || !isReadable())
        return 0;

    fileData->fileStream.getline(data, maxlen);
    return fileData->fileStream.gcount();
}

TpString TpFile::readLine(uint64_t maxlen)
{
    TpFileData *fileData = (TpFileData *)this->data_;
    if (!fileData)
        return false;

    if (!isOpen() || !isReadable())
        return "";

    TpString line;
    if (maxlen > 0)
    {
        line.resize(maxlen);
        fileData->fileStream.getline(&line[0], maxlen);
        line.resize(fileData->fileStream.gcount());
    }
    else
    {
        std::getline(fileData->fileStream, line);
    }
    return line;
}

uint64_t TpFile::write(const char *data, uint64_t len)
{
    TpFileData *fileData = (TpFileData *)this->data_;
    if (!fileData)
        return 0;

    if (!isOpen() || !isWritable())
        return 0;

    fileData->fileStream.write(data, len);
    return fileData->fileStream.gcount();
}

uint64_t TpFile::write(const char *data)
{
    TpFileData *fileData = (TpFileData *)this->data_;
    if (!fileData)
        return 0;

    if (!isOpen() || !isWritable())
        return 0;

    fileData->fileStream << data;
    return strlen(data);
}

uint64_t TpFile::write(const TpString &data)
{
    TpFileData *fileData = (TpFileData *)this->data_;
    if (!fileData)
        return 0;

    if (!isOpen() || !isWritable())
        return 0;

    fileData->fileStream << data;
    return data.size();
}
