/*///------------------------------------------------------------------------------------------------------------------------//
		
说 明 : 对安装包中的config文件解析写入json文件
日 期 : 2024.8.30

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>	//#include <json-c/json.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include "conf_json.h"
#include "utilslib.h"
#include "install.h"
#include "secret_key.h"

#define BUFFER_SIZE 1024

typedef int (*CallbackInstallConfRW)(tpAppID ,const char *);

//提取json文件中某个对象的值


//对写入一个json数组的对象解析并写入
//values:键值，例如：value1@value2@value3
//seg:分隔符
//key1,...,多个键值
void config_json_array_analysis(char *values, struct json_object *lib_array,const char *seq,int num,...) 
{
	va_list args;         		//
//   va_start(args, num);   		//
	char *key_value;
	
	printf("values=%s\n",values);
	while(key_value=strtok_r(values,seq,&values))
	{	
		printf("key_value=%s\n",key_value);
		if(num==0)
		{
			json_object_array_add(lib_array, json_object_new_string((const char*)key_value));
			continue;
		}
		int i=0;
		struct json_object *obj = json_object_new_object();
		va_start(args, num);   		//
		while(i<num)
		{
			i++;
			char *value_n=strtok_r(key_value,"@",&key_value);
			if(value_n==NULL)
				break;
			printf("value_n=%s\n",value_n);
			char *str = va_arg(args, char*); // 获取下一个参数，参数类型是 char*
			printf("key=%s\n", str); // 打印参数值
			json_object_object_add(obj, str, json_object_new_string((const char*)value_n));
		}
		json_object_array_add(lib_array, obj);
	}
	va_end(args); // 清理 va_list 变量
}

//对写入一个json对象的数据解析
//例子：Chingan <2111956539@qq.com>
void config_json_object_analysis(char *values, struct json_object *object,char *seq,int num,...) 
{
	if(num<=0)
	    return;
	va_list args;         		//
	va_start(args, num);   		//
	char *key_value;
	int i=0;
	printf("values=%s\n",values);
	while(key_value=strtok_r(values,seq,&values))
	{	
		i++;
		printf("key_value=%s\n",key_value);
		if(num<i)
			break;
		char *str = va_arg(args, char*); // 获取下一个参数，参数类型是 char*
		if(!str)
		    break;
		printf("key=%s\n", str); // 打印参数值
		json_object_object_add(object, str,json_object_new_string((const char*)key_value));
		printf("hahah\n");
	}

	va_end(args); // 清理 va_list 变量	
}


//配置信息写入json
int config_add_to_json(PackageExportType type,json_object *export_obj,const char *key,const char *value)
{
	size_t len = strlen(value)+1;
	char *key_value=(char *)malloc(len);
	memcpy(key_value,value,len);
	switch(type)
	{
		case EXPORT_LIBS:
		{
			struct json_object *array=json_object_new_array();
			config_json_array_analysis(key_value, array," ",0);
			json_object_object_add(export_obj, key, array);
			break;
		}
		case EXPORT_DEPEND:
		{
			struct json_object *array=json_object_new_array();
			config_json_array_analysis(key_value, array," ",2,"name","version");
			json_object_object_add(export_obj, key, array);
			break;
		}
		case EXPORT_MUST:
		{
			json_object_object_add(export_obj, key,json_object_new_string((const char*)key_value));
			break;
		}
		default:
			break;
	}
	free(key_value);
	return 0;
}

//配置文件中的export行目解析
int config_export_analysis_json(char *line,json_object *export_obj)
{
	char *key = NULL, *value = NULL;
	key = line + 7;
	value = strchr(key, '=');
	if (!value) {
		fprintf(stderr, "无效的 export 行：%s\n", line);
		return -1;
	}
	*value = '\0';
	value++;
	PackageExportType type;
	if(strcmp(key,"lib")==0){
		type=EXPORT_LIBS;
	}
	else if(strcmp(key,"depend")==0){
		type=EXPORT_DEPEND;
	}
	else if(strcmp(key,"icon")==0 || strcmp(key,"start")==0 ||strcmp(key,"remove")==0){	//icon start remove
		type=EXPORT_MUST;
	}
	config_add_to_json(type,export_obj,key,value);
	return 0;
}

//



//配置文件普通行目解析
int config_keyvalue_analysis_json(char *line,json_object *export_obj)
{
	char *key = NULL, *value = NULL;
	//分离key和value
	key = line;
	value = strchr(key, ':');
	if (!value) {
		fprintf(stderr, "无效的 key value 行：%s\n", line);
		return -1;
	}
	*value = '\0';
	value++;

	if(strcmp(key,"Author")==0)
	{
		struct json_object *obj=json_object_new_object();
		char *end=strchr(value, '>');		//去掉结尾
		if (end)
			*end = '\0';
		config_json_object_analysis(value, obj,(char *)" <",2,"name","email");
		json_object_object_add(export_obj, key, obj);
	}
	else if(strcmp(key,"FileExtension")==0)
	{
		struct json_object *array=json_object_new_array();
		config_json_array_analysis(value, array," ",0);
		json_object_object_add(export_obj, key, array);
	}
	else
	{
		json_object_object_add(export_obj, key,json_object_new_string((const char*)value));
	}
	return 0;
}


//从某个object中递归查找该object中某个key的值
static const char *find_key_from_obj(struct json_object *obj, const char *target_key) {
	if (!obj || !target_key) {
		return NULL;
	}

	// 如果是 JSON 对象，遍历键值对
	if (json_object_get_type(obj) == json_type_object) {
		json_object_object_foreach(obj, key, val) {
			// 匹配目标键
			if (strcmp(key, target_key) == 0) {
				return json_object_get_string(val); // 返回键的值
			}
			// 递归检查嵌套结构
			const char *result = find_key_from_obj(val, target_key);
			if (result) {
				return result;
			}
		}
	}

	// 如果是 JSON 数组，遍历每个元素
	if (json_object_get_type(obj) == json_type_array) {
		int array_len = json_object_array_length(obj);
		for (int i = 0; i < array_len; i++) {
			struct json_object *element = json_object_array_get_idx(obj, i);
			const char *result = find_key_from_obj(element, target_key);
			if (result) {
				return result;
			}
		}
	}
	return NULL;
}


//从未加密的json文件中查找key的值
int find_key_from_file(const char *file_path,const char *key,char *value)
{
	FILE *file = fopen(file_path,"r");
	if(file == NULL)
		return -1;
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *data = (char *)malloc(length + 1);
	if (!data) {
		perror("Failed to allocate memory");
		fclose(file);
		return -1;
	}
	fread(data, 1, length, file);
	fclose(file);
	data[length] = '\0';

	// 解析 JSON 数据
	struct json_object *json_obj = json_tokener_parse(data);
	free(data);

	if (!json_obj) {
		fprintf(stderr, "JSON parsing error\n");
		return -1;
	}
	const char *value_temp = find_key_from_obj(json_obj, key);
	strcpy(value,value_temp);
//	printf("value:%s", value_temp);
	return 0;
}

//读取json对象


//根据key和value查找并删除对应的对象
static int del_object_by_keyvalue(struct json_object *objects_array, const char *key,const char *value) {
	int array_len = json_object_array_length(objects_array);

	for (int i = 0; i < array_len; i++) {
		struct json_object *obj = json_object_array_get_idx(objects_array, i);

		// 获取当前对象中的 "ID" 值
		struct json_object *id_obj;
		if (json_object_object_get_ex(obj, key, &id_obj)) {
			const char *get_value = json_object_get_string(id_obj);


			// 如果 ID 匹配，删除该对象
			if (strncmp(get_value, value,strlen(value))==0) {
				json_object_array_del_idx(objects_array, i, 1);
				return 0; // 返回 0 表示删除成功
			}
		}
	}
	// 如果没有找到匹配的对象，返回 -1 表示失败
	return -1;
}

//当install.conf文件不存在的时候新建该文件
int creat_install_conf(const char *install_path)
{
	FILE *file = fopen(install_path, "w");
	if(file == NULL)
	{
		return -1;
	}
	struct json_object *json_obj;
	struct json_object *app_install;

	// 创建一个新的 JSON 对象
	json_obj = json_object_new_object();
	if (json_obj == NULL) {
		printf("Failed to create new JSON object\n");
		return -1;
	}

	json_object_object_add(json_obj, "numbers", json_object_new_int(0));

	// 添加 "appInstall" 键，值为一个空数组
	app_install = json_object_new_array();
	json_object_object_add(json_obj, "appInstall", app_install);

	// 将 JSON 对象写入文件

	if (json_object_to_file_ext(install_path, json_obj, JSON_C_TO_STRING_PRETTY) < 0) {
		printf("Failed to write JSON to file: %s\n", install_path);
		json_object_put(json_obj);
		return -1;
	}

	printf("JSON data written to: %s\n", install_path);

	// 释放 JSON 对象
	json_object_put(json_obj);

	return 0;

}

//install.conf新增一个应用信息
static int add_appuuid_install(tpAppID uuid,const char *install_path)
{
	creat_install_conf(install_path);
	struct json_object *root = json_object_from_file(install_path);
	if (root == NULL) {
		printf("errr\n");
		return -1;
	}
	struct json_object *objects_array;
	if (!json_object_object_get_ex(root, "appInstall", &objects_array)) {
		fprintf(stderr, "Error: 'objects' array not found.\n");
		json_object_put(root);
		return -1;
	}
	
	struct json_object *new_obj = json_object_new_object();
	json_object_object_add(new_obj, "id", json_object_new_string(uuid.value));
	//继续增加其他key和value

	struct json_object *numbers;
	if (json_object_object_get_ex(root, "numbers", &numbers)) {
		int num=json_object_get_int(numbers);
		num+=1;
		json_object_set_int(numbers,num);
	}

	json_object_array_add(objects_array, new_obj);
//	json_object_to_file(install_path, root);
	json_object_to_file_ext(install_path, root, JSON_C_TO_STRING_PRETTY);
	json_object_put(root);
	
	return 0;
}

//install.conf删除一个应用信息
static int del_appuuid_install(tpAppID uuid,const char *install_path)
{

	struct json_object *root = json_object_from_file(install_path);
	if (root == NULL) {
		return -1;
	}
	struct json_object *objects_array;
	if (!json_object_object_get_ex(root, "appInstall", &objects_array)) {
		fprintf(stderr, "Error: 'objects' array not found.\n");
		json_object_put(root);
		return -1;
	}
	if(del_object_by_keyvalue(objects_array, "id",uuid.value)==0)		//根据id的值查找并删除
	{
		struct json_object *numbers;
		if (json_object_object_get_ex(root, "numbers", &numbers)) {
			int num=json_object_get_int(numbers);
			num-=1;
			json_object_set_int(numbers,num);
		}

		//json_object_to_file(install_path, root);
		json_object_to_file_ext(install_path, root, JSON_C_TO_STRING_PRETTY);
	}
	else
	{
		json_object_put(root);
		return -1;
	}
	json_object_put(root);
	return 0;
}

static int file_lock(int fd)
{
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	if (fcntl(fd, F_SETLKW, &lock) == -1) {
		perror("fcntl");
		return -1;
	}
}

static int file_unlock(int fd)
{
	struct flock lock;
	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
    fcntl(fd, F_SETLK, &lock);
}

static int appuuid_install_safe_rw(tpAppID uuid,const char *install_path,CallbackInstallConfRW callback)
{
	int origFd = open(install_path, O_RDWR);
	if (origFd < 0) {
		perror("open original file");
		return -1;
	}

	// 加独占锁，防止其他进程修改
	if (flock(origFd, LOCK_EX) < 0) {
		perror("flock");
		close(origFd);
		return -1;
	}

	char *tmpTemplate = open_directories_temp(APP_INSTALL_PATH);
	printf("创建临时文件%s\n",tmpTemplate);
	int tmpFd = open(tmpTemplate, O_RDWR | O_CREAT | O_EXCL, 0600);//mkstemp(tmpTemplate);
	if (tmpFd < 0) {
		perror("mkstemp");
		flock(origFd, LOCK_UN);
		close(origFd);
		return -1;
	}

	// 逐块读取原文件并写入临时文件
	char buffer[BUFFER_SIZE];
	ssize_t bytesRead;
	while ((bytesRead = read(origFd, buffer, BUFFER_SIZE)) > 0) 
	{
		ssize_t bytesWritten = write(tmpFd, buffer, bytesRead);
		if (bytesWritten != bytesRead) {
			perror("write temporary file");
			close(tmpFd);
			unlink(tmpTemplate);
			flock(origFd, LOCK_UN);
			close(origFd);
			return -1;
		}
	}
	// 读取完成，关闭原文件
	close(origFd);

	if(callback(uuid,tmpTemplate)<0)
	{
		close(tmpFd);
		flock(origFd, LOCK_UN);
		free(tmpTemplate);
		return -1;
	}


	// 确保数据写入磁盘
	fsync(tmpFd);
	close(tmpFd);

	// 用临时文件原子性替换原文件
	if (rename(tmpTemplate, install_path) != 0) {
		perror("rename");
		unlink(tmpTemplate);
		close_directories_temp(tmpTemplate);
		return -1;
	}

	// 释放锁
	flock(origFd, LOCK_UN);
	free(tmpTemplate);
	return 0;
}

//安全新增应用到install文件
int add_appuuid_install_safe(tpAppID uuid,const char *install_path)
{
	return appuuid_install_safe_rw(uuid,install_path,add_appuuid_install);
}
//安全删除应用从install文件
int del_appuuid_install_safe(tpAppID uuid,const char *install_path)
{
	//del_appuuid_install(uuid,install_path);
	return appuuid_install_safe_rw(uuid,install_path,del_appuuid_install);
}




//不加密写入json对象到文件
int write_json_object_file(json_object *root,const char *file_path)
{
	printf("写入json配置文件:%s\n",file_path);
	FILE *file_j = fopen(file_path, "w");
	if (!file_j) {
		fprintf(stderr,"create or open json file error,path:%s",file_path);
		return -1;
	}

	const char *str_json=json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE);		//转字符串
	printf("%s\n", str_json);	//打印测试
	if(fprintf(file_j, "%s\n", str_json)<0)		//写入文件
	{
		fprintf(stderr,"write to json file error,path:%s",file_path);
		fclose(file_j);
		return -1;
	}

	fclose(file_j);
	return 0;
}


//加密写入json对象到文件
static int write_json_object_file_key(json_object *root,const char *file_path,const unsigned char *key)
{
	FILE *file_j = fopen(file_path, "wb");
    if (!file_j) 
		return -1;

    // 获取 JSON 字符串
    const char *str_json = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE);
    size_t plain_len = strlen(str_json);

    // PKCS#7 填充
    size_t pad_len = AES_BLOCK_SIZE - (plain_len % AES_BLOCK_SIZE);
    size_t enc_len = plain_len + pad_len;
    unsigned char *padded = malloc(enc_len);
    memcpy(padded, str_json, plain_len);
    memset(padded + plain_len, pad_len, pad_len);

    // 生成随机 IV
    unsigned char iv[AES_BLOCK_SIZE];
    if (RAND_bytes(iv, sizeof(iv)) != 1) {
        free(padded);
        fclose(file_j);
        return -1;
    }

    // ========== OpenSSL 3.0 加密核心 ========== [2,7](@ref)
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        free(padded);
        fclose(file_j);
        return -1;
    }
    // 初始化 AES-256-CBC 加密
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(padded);
        fclose(file_j);
        return -1;
    }
    // 执行加密
    unsigned char *encrypted = malloc(enc_len + 2 * AES_BLOCK_SIZE);	//多分配一点预留边界
    int out_len, final_len;
    if (EVP_EncryptUpdate(ctx, encrypted, &out_len, padded, enc_len) != 1 ||
        EVP_EncryptFinal_ex(ctx, encrypted + out_len, &final_len) != 1) 
    {
        EVP_CIPHER_CTX_free(ctx);
        free(padded);
        free(encrypted);
        fclose(file_j);
        return -1;
    }
    int total_cipher_len = out_len + final_len;
    // 写入文件
    if (fwrite(iv, 1, sizeof(iv), file_j) != sizeof(iv) ||
        fwrite(encrypted, 1, total_cipher_len, file_j) != total_cipher_len) 
    {
        EVP_CIPHER_CTX_free(ctx);
        free(padded);
        free(encrypted);
        fclose(file_j);
        return -1;
    }
    // 清理资源
    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_cleanse(padded, enc_len);
    free(padded);
    free(encrypted);
    fclose(file_j);
    return 0;
 
}


static char* read_encrypted_file(const char* file_path, unsigned char** iv, unsigned char** ciphertext, size_t* cipher_len) 
{
    FILE* fp = fopen(file_path, "rb");
    if (!fp) 
		return NULL;
    
    // 读取IV（固定16字节）
    *iv = malloc(AES_BLOCK_SIZE);
    if (fread(*iv, 1, AES_BLOCK_SIZE, fp) != AES_BLOCK_SIZE) {
        free(*iv);
        fclose(fp);
        return NULL;
    }
    
    // 获取密文长度并读取
    fseek(fp, 0, SEEK_END);
    *cipher_len = ftell(fp) - AES_BLOCK_SIZE;
    fseek(fp, AES_BLOCK_SIZE, SEEK_SET);
    
    *ciphertext = malloc(*cipher_len);
    if (fread(*ciphertext, 1, *cipher_len, fp) != *cipher_len) {
        free(*iv);
        free(*ciphertext);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    return "SUCCESS";
}

static char* decrypt_json(const unsigned char* key, const unsigned char* iv, const unsigned char* ciphertext, size_t cipher_len) 
{
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return NULL;
    
    // 初始化解密上下文
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    
    // 分配缓冲区（包含填充移除空间）
    unsigned char* plaintext = malloc(cipher_len + AES_BLOCK_SIZE);
    int out_len1, out_len2;
    
    // 解密操作
    if (EVP_DecryptUpdate(ctx, plaintext, &out_len1, ciphertext, cipher_len) != 1 ||
        EVP_DecryptFinal_ex(ctx, plaintext + out_len1, &out_len2) != 1) 
    {
        free(plaintext);
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    
    int total_len = out_len1 + out_len2;
    EVP_CIPHER_CTX_free(ctx);
    
    // 移除PKCS#7填充
    unsigned char pad_value = plaintext[total_len - 1];
    if (pad_value > 0 && pad_value <= AES_BLOCK_SIZE) {
        total_len -= pad_value;
    }
    
    // 转换为字符串
    char* json_str = malloc(total_len + 1);
    memcpy(json_str, plaintext, total_len);
    json_str[total_len] = '\0';
    
    free(plaintext);
    return json_str;
}

static char* read_json_string_file_key(const char* file_path, const unsigned char* key) {
    unsigned char *iv = NULL, *ciphertext = NULL;
    size_t cipher_len = 0;
    
    // 读取文件并提取数据
    if (!read_encrypted_file(file_path, &iv, &ciphertext, &cipher_len)) {
        return NULL;
    }
    
    // 解密为JSON字符串
    char* json_str = decrypt_json(key, iv, ciphertext, cipher_len);
    
    // 清理资源
    free(iv);
    free(ciphertext);

	printf("json\n%s",json_str);
    return json_str;
}


//加密写入json文件
int write_json_object_file_encryption(json_object *root,const char *file_path)
{
	secret_update_key();		//更新密钥

	unsigned char *key=secret_get_key();
	if(!key)
	{
		fprintf(stderr,"get key error\n");
		return -1;
	}
	printf("[debug]:secret_get_key ok \n");
	write_json_object_file_key(root,file_path,key);
	return 0;
}


//从加密的json文件读取字符串json
char *read_json_string_file_encryption(const char *file_path)
{
	unsigned char *key=secret_get_key();
	if(!key)
	{
		fprintf(stderr,"get key error\n");
		return NULL;
	}
	return read_json_string_file_key(file_path,key);
}

