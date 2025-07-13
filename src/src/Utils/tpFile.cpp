#include "tpFile.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
#include <cstring>

struct tpFileData
{
    tpString fileName;

    std::fstream fileStream;
    tpFile::OpenModeFlag openMode;

    tpFileData() : fileName(""), openMode(tpFile::NotOpen)
    {
    }
};

std::ios::openmode convertOpenMode(tpFile::OpenModeFlag mode)
{
    std::ios::openmode iosMode = std::ios::in | std::ios::out;

    switch (mode)
    {
    case tpFile::ReadOnly:
        iosMode = std::ios::in;
        break;
    case tpFile::WriteOnly:
        iosMode = std::ios::out;
        break;
    case tpFile::ReadWrite:
        iosMode = std::ios::in | std::ios::out;
        break;
    case tpFile::Append:
        iosMode = std::ios::app;
        break;
    // case tpFile::Truncate:
    //     iosMode = std::ios::trunc;
    //     break;
    // case tpFile::Text:
    //     iosMode = std::ios::ate;
    //     break;
    default:
        break;
    }

    return iosMode;
}

tpFile::tpFile()
{
    this->data_ = new tpFileData();
}

tpFile::tpFile(const tpString &_fileName)
{
    this->data_ = new tpFileData();

    setFileName(_fileName);
}

tpFile::~tpFile()
{
}

bool tpFile::exists(const tpString &fileName)
{
    if (fileName.empty())
        return false;

    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

bool tpFile::rename(const tpString &oldName, const tpString &newName)
{
    if (!tpFile::exists(oldName))
        return false;

    return (std::rename(oldName.c_str(), newName.c_str()) == 0);
}

bool tpFile::copy(const tpString &fileName, const tpString &newName)
{
    if (!tpFile::exists(fileName))
        return false;

    std::ifstream src(fileName, std::ios::binary);
    std::ofstream dst(newName, std::ios::binary);

    dst << src.rdbuf();
    return dst.good();
}

bool tpFile::remove(const tpString &fileName)
{
    if (!tpFile::exists(fileName))
        return false;

    return (std::remove(fileName.c_str()) == 0);
}

tpString tpFile::fileName() const
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return "";

    return data->fileName;
}

void tpFile::setFileName(const tpString &name)
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return;

    data->fileName = name;
}

tpFileInfo tpFile::fileInfo()
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return tpFileInfo();

    tpFileInfo fileInfo(data->fileName);
    return fileInfo;
}

bool tpFile::exists() const
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data || data->fileName.empty())
        return false;

    struct stat buffer;
    return (stat(data->fileName.c_str(), &buffer) == 0);

    return false;
}

bool tpFile::remove()
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data || data->fileName.empty())
        return false;

    return (std::remove(data->fileName.c_str()) == 0);
}

bool tpFile::rename(const tpString &newName)
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data || data->fileName.empty())
        return false;

    return (std::rename(data->fileName.c_str(), newName.c_str()) == 0);
}

bool tpFile::copy(const tpString &newName)
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data || data->fileName.empty())
        return false;

    std::ifstream src(data->fileName, std::ios::binary);
    std::ofstream dst(newName, std::ios::binary);

    dst << src.rdbuf();
    return dst.good();
}

uint64_t tpFile::size() const
{
    tpFileData *data = (tpFileData *)this->data_;
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

bool tpFile::open(OpenModeFlag mode)
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return false;

    if (isOpen())
        close();

    if (mode == tpFile::NotOpen)
        return false;

    data->openMode = mode;
    data->fileStream.open(data->fileName, convertOpenMode(mode));
    return data->fileStream.is_open();
}

bool tpFile::isOpen() const
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return false;

    return data->fileStream.is_open();
}

bool tpFile::isReadable() const
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return false;

    bool readAble = ((data->openMode & tpFile::ReadOnly) != 0) || ((data->openMode & tpFile::ReadWrite) != 0);

    return (data->openMode & tpFile::ReadOnly) != 0;
}

bool tpFile::isWritable() const
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return false;

    bool writeAble = ((data->openMode & tpFile::WriteOnly) != 0) || ((data->openMode & tpFile::ReadWrite) != 0) || ((data->openMode & tpFile::Append) != 0);

    return writeAble;
}

void tpFile::close()
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return;

    if (isOpen())
    {
        data->fileStream.close();
        data->openMode = tpFile::NotOpen;
    }
}

uint64_t tpFile::pos() const
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return 0;

    return static_cast<uint64_t>(data->fileStream.tellg());
}

bool tpFile::seek(uint64_t offset)
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return false;

    if (!isOpen())
        return false;

    data->fileStream.seekg(offset);
    data->fileStream.seekp(offset);
    return true;

    return false;
}

bool tpFile::atEnd() const
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return false;

    return data->fileStream.eof();
}

bool tpFile::flush()
{
    tpFileData *data = (tpFileData *)this->data_;
    if (!data)
        return false;

    if (isOpen())
    {
        data->fileStream.flush();
        return data->fileStream.good();
    }

    return false;
}

uint64_t tpFile::read(char *data, uint64_t maxlen)
{
    tpFileData *fileData = (tpFileData *)this->data_;
    if (!fileData)
        return false;

    if (!isOpen() || !isReadable())
        return 0;

    fileData->fileStream.read(data, maxlen);
    return fileData->fileStream.gcount();
}

tpString tpFile::read(uint64_t maxlen)
{
    tpFileData *fileData = (tpFileData *)this->data_;
    if (!fileData)
        return "";

    if (!isOpen() || !isReadable())
        return "";

    tpString buffer;
    buffer.resize(maxlen);
    fileData->fileStream.read(&buffer[0], maxlen);
    buffer.resize(fileData->fileStream.gcount());
    return buffer;
}

tpString tpFile::readAll()
{
    tpFileData *fileData = (tpFileData *)this->data_;
    if (!fileData)
        return "";

    if (!isOpen() || !isReadable())
        return "";

    tpString buffer;
    std::streampos pos = fileData->fileStream.tellg();
    fileData->fileStream.seekg(0, std::ios::end);
    buffer.resize(fileData->fileStream.tellg() - pos);
    fileData->fileStream.seekg(pos);
    fileData->fileStream.read(&buffer[0], buffer.size());
    return buffer;
}

uint64_t tpFile::readLine(char *data, uint64_t maxlen)
{
    /*
        std::fstream的getline方法会在读取的字符串末尾自动添加一个空字符，
        所以在使用readLine(char *data, uint64_t maxlen)时，
        maxlen应该至少比要读取的行多一个字符，以容纳空字符
    */
    tpFileData *fileData = (tpFileData *)this->data_;
    if (!fileData)
        return 0;

    if (!isOpen() || !isReadable())
        return 0;

    fileData->fileStream.getline(data, maxlen);
    return fileData->fileStream.gcount();
}

tpString tpFile::readLine(uint64_t maxlen)
{
    tpFileData *fileData = (tpFileData *)this->data_;
    if (!fileData)
        return false;

    if (!isOpen() || !isReadable())
        return "";

    tpString line;
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

uint64_t tpFile::write(const char *data, uint64_t len)
{
    tpFileData *fileData = (tpFileData *)this->data_;
    if (!fileData)
        return 0;

    if (!isOpen() || !isWritable())
        return 0;

    fileData->fileStream.write(data, len);
    return fileData->fileStream.gcount();
}

uint64_t tpFile::write(const char *data)
{
    tpFileData *fileData = (tpFileData *)this->data_;
    if (!fileData)
        return 0;

    if (!isOpen() || !isWritable())
        return 0;

    fileData->fileStream << data;
    return strlen(data);
}

uint64_t tpFile::write(const tpString &data)
{
    tpFileData *fileData = (tpFileData *)this->data_;
    if (!fileData)
        return 0;

    if (!isOpen() || !isWritable())
        return 0;

    fileData->fileStream << data;
    return data.size();
}
