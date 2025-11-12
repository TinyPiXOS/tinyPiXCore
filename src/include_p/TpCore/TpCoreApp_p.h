#ifndef __TP_VARIANT_P_H
#define __TP_VARIANT_P_H

#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <sstream>
#include <iomanip>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include "TpMessage.h"

#include <TpObject.h>
#include <TpCoreApp.h>

// app对象指针；用于TpApp和TpCoreApp静态函数统一
static TpCoreApp *appPtr = nullptr;

struct TpCoreAppData
{
    // 当前应用的所有二级节点
    TpList<TpObject *> objectList;

    bool running = false;
    bool waitRun = false;

    // 主线程ID
    std::thread::id mainThreadId;

    std::mutex gMutex;

    TpMessage *message;

    // 信号槽缓存队列
    std::mutex queueSlotMutex_;
    std::queue<std::function<void()>> slotTasks_;
};

static char runAppLockFileName[PATH_MAX] = {0};

/// @brief 无界面程序退出时资源回收函数
// static void coreAppSignalHandle(int signal)
// {
//     int fd = open(runAppLockFileName, O_RDWR | O_CREAT, 0644);
//     int32_t lockResult = lockf(fd, F_ULOCK, 0);

//     if (fd >= 0)
//     {
//         // 当关闭文件描述符后，文件内容真正被删除
//         close(fd);

//         // 立即删除文件，但文件描述符仍然有效
//         unlink(runAppLockFileName);

//         std::cout << "移除锁文件：" << runAppLockFileName << std::endl;
//     }
// }

/**
 * @brief 将字符串转换为 MD5 哈希值
 * @param input 输入字符串
 * @return 32位小写MD5哈希字符串
 */
static TpString stringToMD5(const TpString &input)
{

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    // OpenSSL 3.0+ 使用 EVP API
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_length;

    if (!context ||
        EVP_DigestInit_ex(context, EVP_md5(), nullptr) != 1 ||
        EVP_DigestUpdate(context, input.c_str(), input.length()) != 1 ||
        EVP_DigestFinal_ex(context, digest, &digest_length) != 1)
    {
        if (context)
            EVP_MD_CTX_free(context);
        return TpString();
    }

    EVP_MD_CTX_free(context);
#else
    // OpenSSL 1.1.x 及更早版本使用传统 API
    MD5_CTX context;
    unsigned char digest[MD5_DIGEST_LENGTH];

    MD5_Init(&context);
    MD5_Update(&context, input.c_str(), input.length());
    MD5_Final(digest, &context);
#endif

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }

    return ss.str();
}

/**
 * @brief 将字符串转换为 MD5 哈希值（大写）
 * @param input 输入字符串
 * @return 32位大写MD5哈希字符串
 */
static TpString stringToMD5Upper(const TpString &input)
{
    TpString md5 = stringToMD5(input);
    for (char &c : md5)
    {
        c = std::toupper(c);
    }
    return md5;
}

static inline bool holdAppSecondRun(const char *runPath, const char *uuid)
{
    int32_t fd;
    int32_t lockResult;
    struct flock lock;
    sprintf(runAppLockFileName, "%s/.%s", runPath, uuid);

    fd = open(runAppLockFileName, O_RDWR | O_CREAT, 0644);

    if (fd < 0)
    {
        return true;
    }

    lockResult = lockf(fd, F_TEST, 0);
    if (lockResult < 0)
    {
        return true;
    }

    lockResult = lockf(fd, F_LOCK, 0);
    if (lockResult < 0)
    {
        return true;
    }

    return false;
}

// 确保应用单例运行
static inline bool decideRunOnce(const char *appName)
{
    char *currentPath = get_current_dir_name();
    if (currentPath == nullptr)
    {
        return false;
    }

    char tempPath[PATH_MAX] = {0};
    sprintf(tempPath, "%s/%s", currentPath, appName);

    bool result = holdAppSecondRun(currentPath, stringToMD5Upper(appName).c_str());
    free(currentPath);

    return result;
}

#endif