/*///------------------------------------------------------------------------------------------------------------------------//
		文件/目录等相关处理的通用功能接口
说 明 : 
日 期 : 2025.3.6

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include "fileUtils.h"



//递归创建目录
int mkdir_path(const char *path, mode_t mode) 
{
	if (path == NULL || *path == '\0') {
        fprintf(stderr, "错误: 路径无效\n");
        return -1;
    }

    // 动态分配 temp 存储路径
    char *temp = strdup(path);  // 复制 path
    if (temp == NULL) {
        perror("malloc 失败");
        return -1;
    }

    struct stat statbuf;
    char *p = NULL;

    // 去除路径末尾的 '/'（避免影响路径解析）
    size_t len = strlen(temp);
    if (len > 1 && temp[len - 1] == '/')
        temp[len - 1] = '\0';

    // 检查目录是否已存在
    if (stat(temp, &statbuf) == 0) {
        if (S_ISDIR(statbuf.st_mode)) {
            free(temp);
            return 0;  // 目录已存在
        } else {
            fprintf(stderr, "错误: %s 已存在但不是目录\n", temp);
            free(temp);
            return -1;
        }
    }

    // 递归创建上级目录
    for (p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';  // 截断路径，临时创建父目录
            if (stat(temp, &statbuf) != 0) {  // 不存在则创建
                if (mkdir(temp, mode) != 0 && errno != EEXIST) {
                    perror("mkdir 失败");
                    free(temp);
                    return -1;
                }
            } else if (!S_ISDIR(statbuf.st_mode)) {
                fprintf(stderr, "错误: %s 已存在但不是目录\n", temp);
                free(temp);
                return -1;
            }
            *p = '/';  // 恢复路径
        }
    }

    // 创建最终目录
    if (mkdir(temp, mode) != 0 && errno != EEXIST) {
        perror("mkdir 失败");
        free(temp);
        return -1;
    }

    free(temp);
    return 0;
}


//递归删除文件夹或文件
//如果是文件夹会递归删除，如果是文件会直接删除
int remove_dir(const char *path) 
{
	struct stat statbuf;

	// 获取文件/目录信息
	if (stat(path, &statbuf) != 0) {
		perror("stat 失败");
		return -1;
	}

	// 如果是普通文件，直接删除
	if (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode)) {
		if (unlink(path) == 0) {
			printf("文件 %s 删除成功\n", path);
		} 
		else {
			perror("unlink 失败");
		}
		return 0;
	}

	// 如果是目录，递归删除
	if (S_ISDIR(statbuf.st_mode)) {
		struct dirent *entry;
		char fullpath[1024];

		DIR *dir = opendir(path);
		if (!dir) {
			perror("opendir 失败");
			return -1;
		}

		while ((entry = readdir(dir)) != NULL) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;  // 跳过 . 和 ..

			snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
			remove_dir(fullpath);  // 递归删除子目录或文件
		}
		closedir(dir);

		// 删除空目录
		if (rmdir(path) != 0) {
			perror("rmdir 失败");
		}
	}
	return 0;
}

//判断是否是目录（）
//返回1是目录，返回0不是目录，返回-1错误
int is_directory(const char *path) 
{
	struct stat statbuf;
	if (stat(path, &statbuf) != 0) {
		return 0;
	}
	return S_ISDIR(statbuf.st_mode);
}

//建议锁打开文件
//如果已经有进程在使用该文件会打开失败
int open_file_flock(const char *file)
{

}


int close_file_flock(int fd)
{

}

