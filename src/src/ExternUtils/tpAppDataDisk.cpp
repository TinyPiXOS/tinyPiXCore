#include "tpAppDataDisk.h"
#include <dirent.h>
#include <stdlib.h>
#include <cstring>

// 获取路径占有空间
long long get_directory_size(const std::string &dirPath)
{
    long long totalSize = 0;
    struct stat statBuf;
    struct dirent *entry;
    DIR *dir = opendir(dirPath.c_str());
    if (!dir)
    {
        // std::cerr << "Error: Failed to open directory" << std::endl;
        return -1;
    }

    while ((entry = readdir(dir)) != nullptr)
    {
        std::string filePath = dirPath + "/" + entry->d_name;
        // 忽略 "." 和 ".."
        if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0)
            continue;

        if (stat(filePath.c_str(), &statBuf) == 0)
        {
            if (S_ISDIR(statBuf.st_mode))
            {
                // 如果是目录，递归处理
                long long dirSize = get_directory_size(filePath);
                if (dirSize != -1)
                {
                    totalSize += dirSize;
                }
            }
            else if (S_ISREG(statBuf.st_mode))
            {
                // 如果是文件，累加文件大小
                totalSize += statBuf.st_size;
            }
        }
        else
        {
            std::cerr << "Error: Failed to get size for:" << filePath << std::endl;
        }
    }
    closedir(dir);
    return totalSize;
}

tpAppDataDisk::tpAppDataDisk()
{
}
tpAppDataDisk::~tpAppDataDisk()
{
}

long int tpAppDataDisk::getAppDiskSpace(const tpString &uuid)
{
    const tpString path = "/System/app/" + uuid;
    return get_directory_size(path);
}

long int tpAppDataDisk::getAppDataDiskSpace(const tpString &uuid)
{
    const tpString path = "/System/app/temp" + uuid;
    long long size = get_directory_size(path);
    if (size < 0)
        return 0;
    return size;
}

long int tpAppDataDisk::getAllAppDiskSpace()
{
    const tpString path = "/System/app/";
    return get_directory_size(path);
}
