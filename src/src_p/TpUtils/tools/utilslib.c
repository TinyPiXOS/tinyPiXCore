/*///------------------------------------------------------------------------------------------------------------------------//
		一些C接口的二次封装
说 明 : 
日 期 : 2025.3.20

/*///------------------------------------------------------------------------------------------------------------------------//

#include <openssl/evp.h>
#include <openssl/md5.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>
#include "tools/utilslib.h"

//返回较小值

//返回较大值


// 从字符串中删除字符
void delete_char_form_string(char *str, char ch)
{
    char *src = str;
    char *dest = str;
    while (*src != '\0')
    {
        if (*src != ch)
        {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0'; // 添加字符串终止符
}

// 字符串字符替换
void string_char_replace(char *str, char ch_s, char ch_d)
{
    int len = (int)strlen(str);
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '\0')
            break;
        if (str[i] == ch_s)
            str[i] = ch_d;
    }
}

// 全部转换为小写
void string_to_lowercase(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        str[i] = tolower((unsigned char)str[i]); // 转换为小写
    }
}


// HEX和ASCII互转
// 8位转16位
uint16_t hex_to_ascii(uint8_t hex)
{
    uint8_t highNibble = (hex >> 4) & 0x0F; // 高 4 位
    uint8_t lowNibble = hex & 0x0F;         // 低 4 位

    // 高低 4 位分别转换为 ASCII 字符
    uint8_t asciiHigh = highNibble + (highNibble < 10 ? '0' : 'A' - 10);
    uint8_t asciiLow = lowNibble + (lowNibble < 10 ? '0' : 'A' - 10);

    return (asciiHigh << 8) | asciiLow;
}
// 16位转8位
uint8_t ascii_to_hex(uint16_t ascii)
{
    uint8_t asciiHigh = (ascii >> 8) & 0xFF; // 高位 ASCII 字符
    uint8_t asciiLow = ascii & 0xFF;         // 低位 ASCII 字符

    // 将高位和低位 ASCII 字符转换为数字
    uint8_t highNibble = asciiHigh - (asciiHigh >= 'A' ? 'A' - 10 : '0');
    uint8_t lowNibble = asciiLow - (asciiLow >= 'A' ? 'A' - 10 : '0');

    return (highNibble << 4) | lowNibble;
}



// 在目录path下查找是否存在target_directory文件
int find_directory(const char *path, const char *target_directory)
{
    DIR *dir;
    struct dirent *entry;
    uint8_t type;
    if ((dir = opendir(path)) == NULL)
    {
        fprintf(stderr, "opendir %s error\n", path);
        return -1;
    }
    // printf("find %s in %s\n", target_directory,path);
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        if (strcmp(entry->d_name, target_directory) == 0)
        {
            type = entry->d_type;
            closedir(dir);
            return type;
        }
    }
    closedir(dir);
    return 0;
}


// 递归创建目录
int recursion_create_path(const char *path)
{
    char *dir_path = strdup(path);

    char *p = dir_path;
    while (*p)
    {
        if (*p == '/' || *p == '\\')
        {
            char temp = *p;
            *p = '\0';
            // 创建当前层级目录（忽略已存在错误）
            mkdir(dir_path, 0777); // 权限设为最大可移植性
            *p = temp;
        }
        p++;
    }
    mkdir(dir_path, 0777); // 创建最末级目录

    free(dir_path);
    return 0;
}


// 打开文件(会递归创建不存在的目录)
FILE *safe_fopen(const char *path, const char *mode)
{
    // 1. 检查路径有效性
    if (!path || !mode)
        return NULL;

    // 2. 分离目录和文件名
    char *dir_path = strdup(path);
    char *last_slash = strrchr(dir_path, '/'); // Linux路径分隔符
#if defined(_WIN32)
    if (!last_slash)
        last_slash = strrchr(dir_path, '\\'); // Windows支持
#endif
    if (last_slash)
        *last_slash = '\0'; // 截断目录部分
    else
    {
        free(dir_path);
        return fopen(path, mode); // 无目录则直接打开
    }

    // 3. 递归创建目录
    char *p = dir_path;
    while (*p)
    {
        if (*p == '/' || *p == '\\')
        {
            char temp = *p;
            *p = '\0';
            // 创建当前层级目录（忽略已存在错误）
            mkdir(dir_path, 0777); // 权限设为最大可移植性
            *p = temp;
        }
        p++;
    }
    mkdir(dir_path, 0777); // 创建最末级目录

    free(dir_path);
    // 4. 打开目标文件
    return fopen(path, mode);
}


// 执行系统命令
int system_command(const char *command)
{
    if (system(command) != 0)
    {
        printf("命令 %s 执行失败\n", command);
        return -1;
    }
    return 0;
}

// 删除文件夹
int system_remove_file(const char *path)
{
    size_t len = strlen(path) + 10;
    char *command = (char *)malloc(len);
    snprintf(command, len, "rm -r %s", path);
    if (system_command(command) < 0)
    {
        free(command);
        return -1;
    }
    free(command);
    return 0;
}

// 使用系统命令进行文件拷贝
int system_copy_file(const char *path_s, const char *path_d)
{
    int ret = 0;
    size_t len = strlen(path_s) + strlen(path_d) + 10;
    char *command = (char *)malloc(len);
    if (command == NULL)
    {
        fprintf(stderr, "Error:malloc error\n");
        return -1;
    }
    snprintf(command, len, "cp -rp %s %s", path_s, path_d);
    printf("command: %s\n", command);
    ret = system(command);
    if (ret != 0)
    {
        fprintf(stderr, "Error:system copy error\n");
        ret = -1;
    }
    free(command);
    return ret;
}


#define BUFFER_SIZE 1024
// 计算文件的MD5哈希值
// file_path：文件路径及名字
// 文件计算得到的MD5值
int compute_md5(const char *file_path, uint8_t output[MD5_DIGEST_LENGTH])
{
    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        perror("Unable to open file");
        return -1;
    }
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx)
    {
        fclose(file);
        fprintf(stderr, "Failed to create EVP MD context\n");
        return -1;
    }

    // 初始化摘要上下文，使用 MD5 算法
    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1)
    {
        fclose(file);
        EVP_MD_CTX_free(mdctx);
        fprintf(stderr, "Failed to initialize digest\n");
        return -1;
    }

    uint8_t buffer[BUFFER_SIZE];
    size_t bytesRead = 0;
    unsigned int digest_len = 0;

    // 读取文件并更新摘要
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        if (EVP_DigestUpdate(mdctx, buffer, bytesRead) != 1)
        {
            fclose(file);
            EVP_MD_CTX_free(mdctx);
            fprintf(stderr, "Failed to update digest\n");
            return -1;
        }
    }

    // 获取最终的摘要值
    if (EVP_DigestFinal_ex(mdctx, output, &digest_len) != 1)
    {
        fclose(file);
        EVP_MD_CTX_free(mdctx);
        fprintf(stderr, "Failed to finalize digest\n");
        return -1;
    }

    EVP_MD_CTX_free(mdctx);
#else
    MD5_CTX md5;
    MD5_Init(&md5);

    uint8_t buffer[BUFFER_SIZE];
    size_t bytesRead = 0;

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        MD5_Update(&md5, buffer, bytesRead);
    }

    MD5_Final(output, &md5);
#endif
    fclose(file);
    return 0;
}
// 将MD5哈希值转换为十六进制字符串
void md5_to_string(uint8_t hash[MD5_DIGEST_LENGTH], char output[33])
{
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[32] = 0; // Null terminate the string
}



double limit_min_max(double value, double min, double max)
{
    if (value > max)
        value = max;
    else if (value < min)
        value = min;
    return value;
}
