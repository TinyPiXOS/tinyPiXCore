#ifndef _UTILS_LIB_H_
#define _UTILS_LIB_H_


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
#include "md5.h"
#include "../inc/appmanage_conf.h"

void delete_end_space(char *str);
void trim_newline(char *str);
void delete_char_form_string(char *str,char ch);
int string_to_version(const char *str,struct tpVersion *ver);
void string_char_replace(char *str,char ch_s,char ch_d);
void string_to_lowercase(char *str) ;
int string_to_number(const char *str,long int *num) ;
char *open_directories_temp(const char *path);
int close_directories_temp(char *file);
void extract_key_value(const char *conf_file,const char *key ,char *value);
//在目录path下查找是否存在target_directory文件
int find_directory(const char *path, const char *target_directory);
int system_command(const char *command);
int is_valid_uuid(const char *uuid);
void uuid_remove_hyphens(const char *input,char *output);
void uuid_add_hyphens(const char *input, char *output);
int compute_md5(const char *file_path, uint8_t output[MD5_DIGEST_LENGTH]);
int MD5_Test();
int del_md5_from_file(const char *file_path,uint8_t md5[MD5_DIGEST_LENGTH],uint8_t flag);
int add_md5_to_file(const char *file_path,uint8_t md5[MD5_DIGEST_LENGTH]);
int remove_file(const char *path);
int system_copy_file(const char *path_s,const char *path_d);
FILE* safe_fopen(const char *path, const char *mode);
char *open_directories_temp_file_name(const char *path,const char *file_name);
int recursion_create_path(const char *path);
#endif