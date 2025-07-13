/*///------------------------------------------------------------------------------------------------------------------------//
		应用安装程序通用解析或处理接口函数
说 明 : 
日 期 : 2024.9.26

/*///------------------------------------------------------------------------------------------------------------------------//


#include "md5.h"
#include "../inc/appmanage_conf.h"
#include "utilslib.h"


//删除换行符
void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

//删除末尾的所有多余空格
void delete_end_space(char *str)
{
	size_t len = strlen(str);
	while(len)
	{
		if (str[len - 1] == ' ') {
       		str[len - 1] = '\0';
    	}

		else
			break;
		len--;
	}
}

//从字符串中删除字符
void delete_char_form_string(char *str,char ch) {
    char *src = str;
    char *dest = str;
    while (*src != '\0') {
        if (*src != ch) {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0'; // 添加字符串终止符
}

//字符串字符替换
void string_char_replace(char *str,char ch_s,char ch_d) {
    int len = (int)strlen(str);
	for(int i=0;i<len;i++)
	{
		if(str[i] == '\0')
			break;
		if(str[i] == ch_s)
			str[i]=ch_d;
	}
}

//全部转换为小写
void string_to_lowercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower((unsigned char)str[i]); // 转换为小写
    }
}

//字符串转长整型
int string_to_number(const char *str,long int *num) {
	char *endptr;
	*num = strtol(str, &endptr, 10);
	// 检查是否完全转换
	if (*endptr != '\0') {
		return -1;
	}
	return 0;
}

//字符串转版本号
int string_to_version(const char *str,struct tpVersion *ver){
	unsigned int x,y,z;
	int ret= sscanf(str, "%u.%u.%u", &x, &y, &z);
	if(ret!=3)
		return -1;
	ver->x=x;
	ver->y=y;
	ver->z=z;
	return 3;
}

//HEX和ASCII互转
//8位转16位
uint16_t hex_to_ascii(uint8_t hex) 
{
    uint8_t highNibble = (hex >> 4) & 0x0F;  // 高 4 位
    uint8_t lowNibble = hex & 0x0F;          // 低 4 位

    // 高低 4 位分别转换为 ASCII 字符
    uint8_t asciiHigh = highNibble + (highNibble < 10 ? '0' : 'A' - 10);
    uint8_t asciiLow = lowNibble + (lowNibble < 10 ? '0' : 'A' - 10);

    return (asciiHigh << 8) | asciiLow;
}
//16位转8位
uint8_t ascii_to_hex(uint16_t ascii) 
{
    uint8_t asciiHigh = (ascii >> 8) & 0xFF;  // 高位 ASCII 字符
    uint8_t asciiLow = ascii & 0xFF;          // 低位 ASCII 字符

    // 将高位和低位 ASCII 字符转换为数字
    uint8_t highNibble = asciiHigh - (asciiHigh >= 'A' ? 'A' - 10 : '0');
    uint8_t lowNibble = asciiLow - (asciiLow >= 'A' ? 'A' - 10 : '0');

    return (highNibble << 4) | lowNibble;
}


//在path目录创建临时文件,（）
char *open_directories_temp(const char *path)
{
	return open_directories_temp_file_name(path,NULL);
}

char *open_directories_temp_file_name(const char *path,const char *file_name)
{
	if(!path)
		return NULL;
	pid_t pid = getpid(); // 获取进程 ID
	time_t t = time(NULL); // 获取当前时间
	char *filename = (char *)malloc(1024); // 分配内存
	if (filename == NULL) {
		perror("malloc");
		return NULL;
	}
	if(file_name)
	{
		//snprintf(filename, 1024, "%s/tmppath_%d_%ld", path,pid, t);
		//recursion_create_path(filename);
		snprintf(filename, 1024, "%s/tmpfile_%d_%ld_%s", path,pid, t,file_name);
	}
	else
		snprintf(filename, 1024, "%s/tmpfile_%d_%ld.tmp", path,pid, t);
	return filename;
}
//删除临时文件
int close_directories_temp(char *file)
{
	if(!file)
		return 0;
	if (remove(file)!=0) {
        perror("remove() error");
	}
	free(file);
	file=NULL;
	return 0;
}


// 配置文件关键字的值提取
void extract_key_value(const char *conf_file,const char *key ,char *value) {
    FILE *file = fopen(conf_file, "r");
    if (file == NULL) {
        perror("Could not open file");
        return;
    }

    char line[CONFIG_MAX_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        // 移除换行符
        trim_newline(line);

        // 查找行中是否包含 "key:"
        char *uuid_pos = strstr(line, key);
        if (uuid_pos != NULL) {
            // 提取 uuid 值
            char *value_pos = uuid_pos + strlen(key);
			strcpy(value,value_pos);
            printf("found: %s:%s\n", key,value);
        }
    }
    fclose(file);
}

//在目录path下查找是否存在target_directory文件
int find_directory(const char *path, const char *target_directory) 
{
	DIR *dir;
	struct dirent *entry;
	uint8_t type; 
	if ((dir = opendir(path)) == NULL) {
		fprintf(stderr,"opendir %s error\n",path);
		return -1;
	}
	//printf("find %s in %s\n", target_directory,path);
	while ((entry = readdir(dir)) != NULL) {
		// Skip "." and ".."
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		if (strcmp(entry->d_name, target_directory) == 0) {
			type=entry->d_type;
			closedir(dir);
			return type;
		}
	}
	closedir(dir);
	return 0;
}


//执行系统命令
int system_command(const char *command)
{
	if(system(command)!=0)
	{	
		printf("命令 %s 执行失败\n", command);
		return -1;
	}
	return 0;
}

//判断字符串是否是标准UUID
int is_valid_uuid(const char *uuid) 
{
	// UUID的正则表达式
	const char *uuid_pattern = "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$";
	regex_t regex;
	int result;

	// 编译正则表达式
	result = regcomp(&regex, uuid_pattern, REG_EXTENDED | REG_NOSUB);
	if (result) {
		fprintf(stderr, "Could not compile regex\n");
		return 0;
	}

	// 使用正则表达式进行匹配
	result = regexec(&regex, uuid, 0, NULL, 0);
	regfree(&regex);

	// 如果匹配成功则返回1，否则返回0
	return result == 0;
}

//删除UUID中的分隔符
void uuid_remove_hyphens(const char *input,char *output){
	const char *src = input;
    char *dest = output;
    
    while (*src) {
        if (*src != '-') {
            *dest = *src;
            dest++;
        }
        src++;
    }
    *dest = '\0'; // Null-terminate the output string
}

//向没有分隔符的UUID中增加分隔符
void uuid_add_hyphens(const char *input, char *output) {
    // UUID format with hyphens: 8-4-4-4-12
    const int positions[] = {8, 4, 4, 4, 12};
    int input_index = 0;
    char *dest = output;

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < positions[i]; ++j) {
            *dest = input[input_index];
            dest++;
            input_index++;
        }
        if (i < 4) {
            *dest = '-';
            dest++;
        }
    }
    *dest = '\0'; // Null-terminate the output string
}

#define BUFFER_SIZE 1024
// 计算文件的MD5哈希值
//file_path：文件路径及名字
//文件计算得到的MD5值
int compute_md5(const char *file_path, uint8_t output[MD5_DIGEST_LENGTH]) 
{
	FILE *file = fopen(file_path, "rb");
	if (!file) {
		perror("Unable to open file");
		return -1;
	}

	MD5_CTX md5;
	MD5_Init(&md5);

	uint8_t buffer[BUFFER_SIZE];
	size_t bytesRead = 0;

	while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
		MD5_Update(&md5, buffer, bytesRead);
	}

	MD5_Final(output, &md5);
	fclose(file);
	return 0;
}

// 将MD5哈希值转换为十六进制字符串
void md5_to_string(uint8_t hash[MD5_DIGEST_LENGTH], char output[33]) {
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[32] = 0; // Null terminate the string
}

//从文件末尾读取并删除MD5信息
//此操作会改变原始文件，慎用
//flag:是否删除末尾的MD5，经过测试不删除不会影响正常解包,=0不删除，=1删除
int del_md5_from_file(const char *file_path,uint8_t md5[MD5_DIGEST_LENGTH],uint8_t flag)
{
	uint8_t md5_len=MD5_DIGEST_LENGTH;
	FILE *file = fopen(file_path, "r+");
	if (file == NULL) {
		printf("Failed to open file:%s\n",file_path);
		return -1;
	}
	if(fseek(file, -md5_len, SEEK_END)!=0){
		fclose(file);
		return -2;
	}
	if(md5!=NULL)
	{
		if(fread(md5, 1, md5_len, file)<md5_len){
			fclose(file);
			return -3;
		}
	}

	if(flag==1)
	{
		// 移动到文件末尾并获取文件大小
		if(fseek(file, 0, SEEK_END)!=0){
			fclose(file);
			return -2;
		}

		long file_size = ftell(file);
		if (file_size < md5_len) {
			fclose(file);
			return -4;
		}

		long new_size = file_size - md5_len;
		// 截断文件到新的大小
		if (ftruncate(fileno(file), new_size) != 0) {
			perror("Failed to truncate file");
			fclose(file);
			return -5;
		}
	}
	fclose(file);
	return 0;
}

//向文件末尾增加MD5信息
int add_md5_to_file(const char *file_path,uint8_t md5[MD5_DIGEST_LENGTH])
{
	uint8_t md5_len=MD5_DIGEST_LENGTH;
	FILE *file = fopen(file_path, "a");
	if (file == NULL) {
		perror("Failed to open file");
		return 1;
	}
	if(fseek(file, 0, SEEK_END)!=0){
		fclose(file);
		return -1;
	}
	if(fwrite(md5,1,md5_len,file)!=md5_len){
		fclose(file);
		return -1;
	}

	fclose(file);
	return 0;

}

//删除文件夹
int remove_file(const char *path)
{	
	size_t len=strlen(path)+10;
	char *command=(char *)malloc(len);
	snprintf(command, len,"rm -r %s",path);
	if(system_command(command)<0)
	{
		free(command);
		return -1;
	}
	free(command);
	return 0;
}

//使用系统命令进行文件拷贝
int system_copy_file(const char *path_s,const char *path_d)
{
	int ret=0;
	size_t len=strlen(path_s)+strlen(path_d)+10;
	char *command=(char *)malloc(len);
	if(command==NULL){
		fprintf(stderr,"Error:malloc error\n");
		return -1;
	}
	snprintf(command,len,"cp -rp %s %s",path_s,path_d);
	printf("command: %s\n",command);
	ret = system(command);
	if (ret != 0) {
		fprintf(stderr,"Error:system copy error\n");
		ret=-1;
	}
	free(command);
	return ret;
}

//递归创建目录
int recursion_create_path(const char *path)
{
	char *dir_path=strdup(path);

    char *p = dir_path;
    while (*p) {
        if (*p == '/' || *p == '\\') {
            char temp = *p;
            *p = '\0';
            // 创建当前层级目录（忽略已存在错误）
            mkdir(dir_path, 0777);  // 权限设为最大可移植性
            *p = temp;
        }
        p++;
    }
    mkdir(dir_path, 0777);  // 创建最末级目录

	free(dir_path);
	return 0;
}


//打开文件(会递归创建不存在的目录)
FILE* safe_fopen(const char *path, const char *mode) {
    // 1. 检查路径有效性
    if (!path || !mode) 
		return NULL;
    
    // 2. 分离目录和文件名
    char *dir_path=strdup(path);
    char *last_slash = strrchr(dir_path, '/');  // Linux路径分隔符
#if defined(_WIN32)
    if (!last_slash) last_slash = strrchr(dir_path, '\\');  // Windows支持
#endif
    if (last_slash) 
		*last_slash = '\0';  // 截断目录部分
    else 
	{
		free(dir_path);
		return fopen(path, mode);  // 无目录则直接打开
	}

    // 3. 递归创建目录
    char *p = dir_path;
    while (*p) {
        if (*p == '/' || *p == '\\') {
            char temp = *p;
            *p = '\0';
            // 创建当前层级目录（忽略已存在错误）
            mkdir(dir_path, 0777);  // 权限设为最大可移植性
            *p = temp;
        }
        p++;
    }
    mkdir(dir_path, 0777);  // 创建最末级目录

	free(dir_path);
    // 4. 打开目标文件
    return fopen(path, mode);
}
