#ifndef _APPM_UTILS_H_
#define _APPM_UTILS_H_


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
#include "AppManage/AppmanageConf.h"
#include "tools/utilslib.h"

void delete_end_space(char *str);
void trim_newline(char *str);
FILE *safe_fopen(const char *path, const char *mode);

int string_to_number(const char *str,long int *num) ;
char *open_directories_temp(const char *path);
int close_directories_temp(char *file);
void extract_key_value(const char *conf_file,const char *key ,char *value);


int is_valid_uuid(const char *uuid);
void uuid_remove_hyphens(const char *input,char *output);
void uuid_add_hyphens(const char *input, char *output);

int MD5_Test();
int del_md5_from_file(const char *file_path,uint8_t md5[MD5_DIGEST_LENGTH],uint8_t flag);
int add_md5_to_file(const char *file_path,uint8_t md5[MD5_DIGEST_LENGTH]);

int string_to_version(const char *str, struct TpVersion *ver);

char *open_directories_temp_file_name(const char *path, const char *file_name);





#endif