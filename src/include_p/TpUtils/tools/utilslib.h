//C语言部分通用的库/工具
#ifndef _TP_UTILS_C_LIB_H_
#define _TP_UTILS_C_LIB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <ctype.h>
#include <regex.h>
#include <openssl/md5.h>
#include "utlist.h"					//链表
#include "variable_array.h"			//动态数组
#include <pthread.h>

#define MIN_VALUE(a, b) ((a) < (b) ? (a) : (b))
#define MAX_VALUE(a, b) ((a) > (b) ? (a) : (b))



typedef enum{
	B_FALSE	= 0,
	B_TRUE	= 1
}UtilsBool;

//回调结构体
struct CallbackData{
	void (*callback)(const void *indata,void *userdata);
	void *userdata;
	pthread_mutex_t lock;
};


// 从字符串中删除字符
void delete_char_form_string(char *str, char ch);

// 字符串字符替换
void string_char_replace(char *str, char ch_s, char ch_d);

// 全部转换为小写
void string_to_lowercase(char *str);

//在目录path下查找是否存在target_directory文件
int find_directory(const char *path, const char *target_directory);

// 递归创建目录
int recursion_create_path(const char *path);

// 安全打开文件(会递归创建不存在的目录)
FILE *safe_fopen(const char *path, const char *mode);

// 执行系统命令
int system_command(const char *command);
// 删除文件夹
int system_remove_file(const char *path);
// 使用系统命令进行文件拷贝
int system_copy_file(const char *path_s, const char *path_d);

//计算文件的MD5哈希值
int compute_md5(const char *file_path, uint8_t output[MD5_DIGEST_LENGTH]);


#ifdef __cplusplus
}
#endif

#endif