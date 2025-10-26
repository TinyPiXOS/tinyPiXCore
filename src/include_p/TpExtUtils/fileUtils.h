#ifndef _FILE_UTILS_H_
#define _FILE_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif





//递归创建目录
int mkdir_path(const char *path, mode_t mode);
int remove_dir(const char *path);
int is_directory(const char *path);


#ifdef __cplusplus
}
#endif

#endif