/*///------------------------------------------------------------------------------------------------------------------------//
        应用安装程序通用解析或处理接口函数
说 明 :
日 期 : 2024.9.26

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
#include "AppManage/AppmanageConf.h"
#include "AppManage/AppmUtils.h"
// 字符串转版本号
int TpAppmUtils::StringToVersion(const char *str, struct TpVersion *ver)
{
    unsigned int x, y, z;
    int ret = sscanf(str, "%u.%u.%u", &x, &y, &z);
    if (ret != 3)
        return -1;
    ver->x = x;
    ver->y = y;
    ver->z = z;
    return 3;
}

// 删除换行符
void trim_newline(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
}

// 删除末尾的所有多余空格
void delete_end_space(char *str)
{
    size_t len = strlen(str);
    while (len)
    {
        if (str[len - 1] == ' ')
        {
            str[len - 1] = '\0';
        }

        else
            break;
        len--;
    }
}




// 字符串转长整型
int string_to_number(const char *str, long int *num)
{
    char *endptr;
    *num = strtol(str, &endptr, 10);
    // 检查是否完全转换
    if (*endptr != '\0')
    {
        return -1;
    }
    return 0;
}


char *open_directories_temp_file_name(const char *path, const char *file_name)
{
    if (!path)
        return NULL;
    pid_t pid = getpid();                  // 获取进程 ID
    time_t t = time(NULL);                 // 获取当前时间
    char *filename = (char *)malloc(1024); // 分配内存
    if (filename == NULL)
    {
        perror("malloc");
        return NULL;
    }
    if (file_name)
    {
        // snprintf(filename, 1024, "%s/tmppath_%d_%ld", path,pid, t);
        // recursion_create_path(filename);
        snprintf(filename, 1024, "%s/tmpfile_%d_%ld_%s", path, pid, t, file_name);
    }
    else
        snprintf(filename, 1024, "%s/tmpfile_%d_%ld.tmp", path, pid, t);
    return filename;
}

// 在path目录创建临时文件,（）
char *open_directories_temp(const char *path)
{
    return open_directories_temp_file_name(path, NULL);
}

// 删除临时文件
int close_directories_temp(char *file)
{
    if (!file)
        return 0;
    if (remove(file) != 0)
    {
        perror("remove() error");
    }
    free(file);
    file = NULL;
    return 0;
}

// 配置文件关键字的值提取
void extract_key_value(const char *conf_file, const char *key, char *value)
{
    FILE *file = fopen(conf_file, "r");
    if (file == NULL)
    {
        perror("Could not open file");
        return;
    }

    char line[CONFIG_MAX_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // 移除换行符
        trim_newline(line);

        // 查找行中是否包含 "key:"
        char *uuid_pos = strstr(line, key);
        if (uuid_pos != NULL)
        {
            // 提取 uuid 值
            char *value_pos = uuid_pos + strlen(key);
            strcpy(value, value_pos);
            printf("found: %s:%s\n", key, value);
        }
    }
    fclose(file);
}

// 判断字符串是否是标准UUID
int is_valid_uuid(const char *uuid)
{
    // UUID的正则表达式
    const char *uuid_pattern = "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$";
    regex_t regex;
    int result;

    // 编译正则表达式
    result = regcomp(&regex, uuid_pattern, REG_EXTENDED | REG_NOSUB);
    if (result)
    {
        fprintf(stderr, "Could not compile regex\n");
        return 0;
    }

    // 使用正则表达式进行匹配
    result = regexec(&regex, uuid, 0, NULL, 0);
    regfree(&regex);

    // 如果匹配成功则返回1，否则返回0
    return result == 0;
}

// 删除UUID中的分隔符
void uuid_remove_hyphens(const char *input, char *output)
{
    const char *src = input;
    char *dest = output;

    while (*src)
    {
        if (*src != '-')
        {
            *dest = *src;
            dest++;
        }
        src++;
    }
    *dest = '\0'; // Null-terminate the output string
}

// 向没有分隔符的UUID中增加分隔符
void uuid_add_hyphens(const char *input, char *output)
{
    // UUID format with hyphens: 8-4-4-4-12
    const int positions[] = {8, 4, 4, 4, 12};
    int input_index = 0;
    char *dest = output;

    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < positions[i]; ++j)
        {
            *dest = input[input_index];
            dest++;
            input_index++;
        }
        if (i < 4)
        {
            *dest = '-';
            dest++;
        }
    }
    *dest = '\0'; // Null-terminate the output string
}


// 从文件末尾读取并删除MD5信息
// 此操作会改变原始文件，慎用
// flag:是否删除末尾的MD5，经过测试不删除不会影响正常解包,=0不删除，=1删除
int TpAppmUtils::DelMd5FromFile(const char *file_path, uint8_t md5[MD5_DIGEST_LENGTH], uint8_t flag)
{
    uint8_t md5_len = MD5_DIGEST_LENGTH;
    FILE *file = fopen(file_path, "r+");
    if (file == NULL)
    {
        printf("Failed to open file:%s\n", file_path);
        return -1;
    }
    if (fseek(file, -md5_len, SEEK_END) != 0)
    {
        fclose(file);
        return -2;
    }
    if (md5 != NULL)
    {
        if (fread(md5, 1, md5_len, file) < md5_len)
        {
            fclose(file);
            return -3;
        }
    }

    if (flag == 1)
    {
        // 移动到文件末尾并获取文件大小
        if (fseek(file, 0, SEEK_END) != 0)
        {
            fclose(file);
            return -2;
        }

        long file_size = ftell(file);
        if (file_size < md5_len)
        {
            fclose(file);
            return -4;
        }

        long new_size = file_size - md5_len;
        // 截断文件到新的大小
        if (ftruncate(fileno(file), new_size) != 0)
        {
            perror("Failed to truncate file");
            fclose(file);
            return -5;
        }
    }
    fclose(file);
    return 0;
}

// 向文件末尾增加MD5信息
int TpAppmUtils::AddMd5ToFile(const char *file_path, uint8_t md5[MD5_DIGEST_LENGTH])
{
    uint8_t md5_len = MD5_DIGEST_LENGTH;
    FILE *file = fopen(file_path, "a");
    if (file == NULL)
    {
        perror("Failed to open file");
        return 1;
    }
    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return -1;
    }
    if (fwrite(md5, 1, md5_len, file) != md5_len)
    {
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}



