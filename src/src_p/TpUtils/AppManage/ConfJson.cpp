/*///------------------------------------------------------------------------------------------------------------------------//

说 明 : 对安装包中的config文件解析写入json文件
日 期 : 2024.8.30

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdarg>
#include "ConfJson.h"
#include "AppManage/AppmUtils.h"
#include "Install.h"
#include "SecretKey.h"
#include "TpJsonArray.h"
#include "TpJsonObject.h"
#include "TpJsonDocument.h"
#include "TpFile.h"

#define BUFFER_SIZE 1024

typedef int (*CallbackInstallConfRW)(TpAppID, const TpString &);

// 提取json文件中某个对象的值

//
void config_json_array_analysis(TpString &values, TpJsonArray *lib_array, const TpString &seq, int num, ...)
{
    va_list args;
    va_start(args, num); //
    printf("values:%s,\n",values.c_str());  //./lib 
    size_t pos = values.find(seq); // 查找第一个分隔符的位置
    while( pos != TpString::npos)
    {
        if (num == 0 )
        {
			TpString array_value=values.substr(0,pos);
            if(array_value.empty())
                break;
            lib_array->append(array_value);      
            std::cout<< "append:" << array_value << "," <<std::endl;   
            values=values.substr(pos);
            if(values.empty())
                break;
            pos = values.find(seq); 
            continue;
        }

        //value 里面有子分割符号
        TpJsonObject obj;
        int i = 0;
        while (i < num)
        {
            i++;

            size_t pos_sub = values.find("@"); 
            if(pos_sub != std::string::npos )
                break;

            char *str = va_arg(args, char *); // 获取下一个参数，参数类型是 char*
            if(!str)
                break;

            printf("key=%s\n", str);          // 打印参数值
            //json_object_object_add(obj, str, json_object_new_string((const char *)value_n));
			obj.insert(TpString(str), (TpString)values.substr(0,pos_sub));
            std::cout<< "insert:" << (TpString)values.substr(0,pos_sub) <<std::endl;   
        }
        lib_array->append(obj);
        pos = values.substr(pos).find(seq); 
    }
    va_end(args); // 清理 va_list 变量
}


// 对写入一个json对象的数据解析
// 例子：Chingan <2111956539@qq.com>
//void config_json_object_analysis(char *values, struct json_object *object, char *seq, int num, ...)
/*void config_json_object_analysis(char *values, TpJsonObject *object, char *seq, int num, ...)
{
    if (num <= 0)
        return;
    va_list args;        //
    va_start(args, num); //
    char *key_value;
    int i = 0;
    while (key_value = strtok_r(values, seq, &values))
    {
        i++;
        if (num < i)
            break;
        char *str = va_arg(args, char *); // 获取下一个参数，参数类型是 char*
        if (!str)
            break;
		object->insert(str, key_value);
    }
    va_end(args); // 清理 va_list 变量
}*/

void config_json_object_analysis(TpString &values, TpJsonObject *object, const TpString &seq, int num, ...)
{
	if (num <= 0)
        return;
	va_list args;        //
    va_start(args, num); //
	size_t pos = values.find(seq); // 查找第一个分隔符的位置
	int i = 0;
    char *str=NULL;
    while( pos != TpString::npos )
    {
 		i++;
        if (num < i)
            break;
		TpString key_value = values.substr(0,pos);

		str = va_arg(args, char *); // 获取下一个参数，参数类型是 char*
        if (!str)
            break;	
		object->insert(str, key_value);
        printf("insert:%s %s\n",str,key_value.c_str());
		values = values.substr(pos+seq.length());
		pos = values.find(seq); // 
	}
    if(!values.empty())
    {
        str = va_arg(args, char *); // 获取下一个参数，参数类型是 char*
        if (!str)
            return ;
        object->insert(str, values);
        printf("insert:%s %s\n",str,values.c_str());
    }
	va_end(args); // 清理 va_list 变量
}


// 从某个object中递归查找该object中某个key的值
/*static const char *find_key_from_obj(struct json_object *obj, const char *target_key)
{
    if ( !obj || !target_key)
    {
        return NULL;
    }

    // 如果是 JSON 对象，遍历键值对
    if (json_object_get_type(obj) == json_type_object)
    {
        json_object_object_foreach(obj, key, val)
        {
            // 匹配目标键
            if (strcmp(key, target_key) == 0)
            {
                return json_object_get_string(val); // 返回键的值
            }
            // 递归检查嵌套结构
            const char *result = find_key_from_obj(val, target_key);
            if (result)
            {
                return result;
            }
        }
    }

    // 如果是 JSON 数组，遍历每个元素
    if (json_object_get_type(obj) == json_type_array)
    {
        int array_len = json_object_array_length(obj);
        for (int i = 0; i < array_len; i++)
        {
            struct json_object *element = json_object_array_get_idx(obj, i);
            const char *result = find_key_from_obj(element, target_key);
            if (result)
            {
                return result;
            }
        }
    }
    return NULL;
}*/

TpString find_key_from_obj(const TpJsonObject &obj, const TpString &target_key) {
    
    // 1. 首先检查当前对象是否直接包含目标 key
    if (obj.contains(target_key)) {
        TpJsonValue value = obj.value(target_key);
        // 统一转换为字符串并返回
        return value.toString();
    }
    
    // 2. 如果当前层没找到，递归搜索所有子对象和数组
    TpList<TpString> keyList = obj.keys();
    for (const TpString &key : keyList) {
        TpJsonValue currentValue = obj.value(key);
        
        if (currentValue.isObject()) {
            // 递归搜索子对象
            TpString result = find_key_from_obj(currentValue.toObject(), target_key);
            if (!result.empty()) {
                return result;
            }
        } 
		else if (currentValue.isArray()) {
            // 递归搜索数组中的每个元素
            TpJsonArray array = currentValue.toArray();
            for (uint32_t i = 0; i < array.count(); ++i) {
                TpJsonValue element = array.at(i);
                
                if (element.isObject()) {
                    // 如果数组元素是对象，递归搜索这个对象
                    TpString result = find_key_from_obj(element.toObject(), target_key);
                    if (!result.empty()) {
                        return result;
                    }
                } 
				else if (element.isArray()) {
                    // 如果数组元素本身也是数组，则递归搜索这个子数组
                    // 注意：这里需要处理嵌套数组的情况
                    TpJsonArray subArray = element.toArray();
                    // 由于数组嵌套数组的情况较少见，这里简化处理
                    for (uint32_t j = 0; j < subArray.count(); ++j) {
                        TpJsonValue subElement = subArray.at(j);
                        if (subElement.isObject()) {
                            TpString result = find_key_from_obj(subElement.toObject(), target_key);
                            if (!result.empty()) {
                                return result;
                            }
                        }
                        // 对于更深层的嵌套，可以根据需要继续扩展
                    }
                }
            }
        }
    }
    
    return TpString(); // 未找到，返回空字符串
}


// 读取json对象

// 根据key和value查找并删除对应的对象
//static int del_object_by_keyvalue(struct json_object *objects_array, const char *key, const char *value)
static int del_object_by_keyvalue(TpJsonArray objects_array, const char *key, const char *value)
{
    //int array_len = json_object_array_length(objects_array);
	int array_len = objects_array.count();

    for (int i = 0; i < array_len; i++)
    {
        //struct json_object *obj = json_object_array_get_idx(objects_array, i);
		TpJsonObject obj = objects_array.at(i).toObject();

        // 获取当前对象中的 "ID" 值
        // struct json_object *id_obj;
        // if (json_object_object_get_ex(obj, key, &id_obj))
        // {
        //     const char *get_value = json_object_get_string(id_obj);
        //     // 如果 ID 匹配，删除该对象
        //     if (strncmp(get_value, value, strlen(value)) == 0)
        //     {
        //         json_object_array_del_idx(objects_array, i, 1);
        //         return 0; // 返回 0 表示删除成功
        //     }
        // }
		if(obj.value(TpString(key)).toString()==TpString(value))
		{
			obj.remove(TpString(key));
			return 0;
		}
		
    }
    // 如果没有找到匹配的对象，返回 -1 表示失败
    return -1;
}

// 当install.conf文件不存在的时候新建该文件
int createInstallConfig(const TpString &installPath)
{
    // 创建TpFile对象
    TpFile file(installPath);
    // 以写模式打开文件
    if (!file.open(TpFile::WriteOnly))
        return -1;

    // 创建JSON对象
    TpJsonObject jsonObj;

    // 添加基本字段
    jsonObj.insert("numbers", 0);

    // 添加"appInstall" 键 值为空数组
    TpJsonArray appInstallArray;
    jsonObj.insert("appInstall", appInstallArray);

    // 将JSON对象转换为字符串并写入文件
    TpJsonDocument jsonDoc(jsonObj);
    TpString jsonString = jsonDoc.toJson();
    uint64_t bytesWritten = file.write(jsonString);

    if (bytesWritten == 0)
    {
        file.close();
        return -1;
    }

    file.close();
    return 0;
}

static int file_lock(int fd)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl");
        return -1;
    }

    return 0;
}

static int file_unlock(int fd)
{
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    fcntl(fd, F_SETLK, &lock);

    return 0;
}

// 加密写入json对象到文件
///static int write_json_object_file_key(json_object *root, const TpString &filePath, const unsigned char *key)
static int write_json_object_file_key(TpJsonObject root, const TpString &filePath, const unsigned char *key)
{
    FILE *file_j = fopen(filePath.c_str(), "wb");
    if (!file_j)
        return -1;

    // 获取 JSON 字符串
    //const char *str_json = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE);
    const char *str_json = TpJsonDocument(root).toJson().c_str();
    size_t plain_len = strlen(str_json);

    // PKCS#7 填充
    size_t pad_len = AES_BLOCK_SIZE - (plain_len % AES_BLOCK_SIZE);
    size_t enc_len = plain_len + pad_len;
    unsigned char *padded = static_cast<unsigned char *>(malloc(enc_len));
    memcpy(padded, str_json, plain_len);
    memset(padded + plain_len, pad_len, pad_len);

    // 生成随机 IV
    unsigned char iv[AES_BLOCK_SIZE];
    if (RAND_bytes(iv, sizeof(iv)) != 1)
    {
        free(padded);
        fclose(file_j);
        return -1;
    }

    // ========== OpenSSL 3.0 加密核心 ========== [2,7](@ref)
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        free(padded);
        fclose(file_j);
        return -1;
    }
    // 初始化 AES-256-CBC 加密
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        free(padded);
        fclose(file_j);
        return -1;
    }
    // 执行加密
    unsigned char *encrypted = static_cast<unsigned char *>(malloc(enc_len + 2 * AES_BLOCK_SIZE)); // 多分配一点预留边界
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

//
static const char *read_encrypted_file(const TpString &filePath, unsigned char **iv, unsigned char **ciphertext, size_t *cipher_len)
{
    FILE *fp = fopen(filePath.c_str(), "rb");
    if (!fp)
        return NULL;

    // 读取IV（固定16字节）
    *iv = static_cast<unsigned char *>(malloc(AES_BLOCK_SIZE));
    if (fread(*iv, 1, AES_BLOCK_SIZE, fp) != AES_BLOCK_SIZE)
    {
        free(*iv);
        fclose(fp);
        return NULL;
    }

    // 获取密文长度并读取
    fseek(fp, 0, SEEK_END);
    *cipher_len = ftell(fp) - AES_BLOCK_SIZE;
    fseek(fp, AES_BLOCK_SIZE, SEEK_SET);

    *ciphertext = static_cast<unsigned char *>(malloc(*cipher_len));
    if (fread(*ciphertext, 1, *cipher_len, fp) != *cipher_len)
    {
        free(*iv);
        free(*ciphertext);
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    return "SUCCESS";
}

static char *decrypt_json(const unsigned char *key, const unsigned char *iv, const unsigned char *ciphertext, size_t cipher_len)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return NULL;

    // 初始化解密上下文
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    // 分配缓冲区（包含填充移除空间）
    unsigned char *plaintext = static_cast<unsigned char *>(malloc(cipher_len + AES_BLOCK_SIZE));
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
    if (pad_value > 0 && pad_value <= AES_BLOCK_SIZE)
    {
        total_len -= pad_value;
    }

    // 转换为字符串
    char *json_str = static_cast<char *>(malloc(total_len + 1));
    memcpy(json_str, plaintext, total_len);
    json_str[total_len] = '\0';

    free(plaintext);
    return json_str;
}

static char *read_json_string_file_key(const TpString &filePath, const unsigned char *key)
{
    unsigned char *iv = NULL, *ciphertext = NULL;
    size_t cipher_len = 0;

    // 读取文件并提取数据
    if (!read_encrypted_file(filePath, &iv, &ciphertext, &cipher_len))
    {
        return NULL;
    }

    // 解密为JSON字符串
    char *json_str = decrypt_json(key, iv, ciphertext, cipher_len);

    // 清理资源
    free(iv);
    free(ciphertext);
    
    return json_str;
}

//
int ConfigJsonParser::config_export_analysis_json(const TpString &line_, TpJsonObject &export_obj)
{
    TpString keyvalue=line_.substr(7);
    size_t pos=keyvalue.find("=");
    if(pos == TpString::npos)
    {
        fprintf(stderr, "无效的 export 行：%s\n", line_.c_str());
        return -1;
    }
    TpString key=keyvalue.substr(0,pos);
    TpString value=keyvalue.substr(pos+1);

    PackageExportType type=EXPORT_NONE;
    if (key == TpString("Lib"))
    {
        type = EXPORT_LIBS;
    }
    else if (key == TpString("Depend"))
    {
        type = EXPORT_DEPEND;
    }
    else if ( key == TpString("Icon") || key == TpString("Start") || key == TpString("Remove"))
    { // icon start remove
        type = EXPORT_MUST;
    }
    else
    {}
    configAddToJson(type, export_obj, key, value);

    return 0;
}

int ConfigJsonParser::config_keyvalue_analysis_json(const TpString &line_, TpJsonObject &export_obj)
{
    size_t pos=line_.find(":");
	if(pos == TpString::npos)
	{
        fprintf(stderr, "无效的 key value 行：%s\n", line_.c_str());
		return -1;
	}
    TpString key=line_.substr(0,pos);
    TpString value=line_.substr(pos+1);

	if(key == TpString("Author"))
	{
		TpJsonObject obj;
		size_t pos_end=value.find(">");
		if(pos_end != TpString::npos)
		{
			value=value.substr(0,pos_end);
		}

        config_json_object_analysis(value, &obj, TpString(" <"), 2, "Name", "Email");

		export_obj.insert(key, obj);
	}
    else if (key == TpString("FileExtension"))
    {
		TpJsonArray array;
        config_json_array_analysis(value, &array, " ", 0);
		export_obj.insert(key, array);
    }
    else
    {
		export_obj.insert(key, value);
    }
    return 0;
}

int ConfigJsonParser::findKeyFromFile(const TpString &filePath, const TpString &key, TpString &value)
{
    TpFile file(filePath);
    if (!file.open(TpFile::ReadOnly))
        return -1;

    TpString data = file.readAll();

    // 解析 JSON 数据
    //struct json_object *json_obj = json_tokener_parse(data.c_str());
	TpJsonObject json_obj = TpJsonDocument::fromJson(data).object();
    //if (!json_obj)
	if (json_obj.isEmpty())
    {
        fprintf(stderr, "JSON parsing error\n");
        return -1;
    }

    //const char *value_temp = find_key_from_obj(json_obj, key.c_str());
	TpString value_temp = find_key_from_obj(json_obj, key);

    value = value_temp;
    //	printf("value:%s", value_temp);
    return 0;
}

//向json文件添加配置
int ConfigJsonParser::configAddToJson(PackageExportType type, TpJsonObject &exportObj, const TpString &key, TpString &value)
{
    switch (type)
    {
    case EXPORT_LIBS:
    {
        //struct json_object *array = json_object_new_array();
        //config_json_array_analysis((char *)value.c_str(), array, " ", 0);
        //json_object_object_add(exportObj, key.c_str(), array);
		TpJsonArray array;
		config_json_array_analysis(value, &array, TpString(" "), 0);
		exportObj.insert(key, array);
        //std::cout << "Json:" << TpJsonDocument(array).toJson() << std::endl;
        break;
    }
    case EXPORT_DEPEND:
    {
        //struct json_object *array = json_object_new_array();
        //config_json_array_analysis((char *)value.c_str(), array, " ", 2, "name", "version");
        //json_object_object_add(exportObj, key.c_str(), array);
		TpJsonArray array;
		config_json_array_analysis(value, &array, TpString(" "), 2, "name", "version");
		exportObj.insert(key, array);
        break;
    }
    case EXPORT_MUST:
    {
		//json_object_object_add(exportObj, key.c_str(), json_object_new_string(value.c_str()));
		exportObj.insert(key, value);
        break;
    }
    default:
        break;
    }

    return 0;
}

//int ConfigJsonParser::writeJsonObjectFile(json_object *root, const TpString &filePath)
int ConfigJsonParser::writeJsonObjectFile(TpJsonObject &root, const TpString &filePath)
{
    // printf("写入json配置文件:%s\n", filePath);
    FILE *file_j = fopen(filePath.c_str(), "w");
    if (!file_j)
    {
        fprintf(stderr, "create or open json file error,path:%s", filePath.c_str());
        return -1;
    }

    //const char *str_json = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE); // 转字符串
	
    std::cout << "Json:" << TpJsonDocument(root).toJson() << std::endl;
    if (fprintf(file_j, "%s\n", TpJsonDocument(root).toFormattedJson().c_str()) < 0)
    {
        fprintf(stderr, "write to json file error,path:%s", filePath.c_str());
        fclose(file_j);
        return -1;
    }
    fclose(file_j);
    return 0;
}

//
//int ConfigJsonParser::writeJsonObjectFileEncryption(json_object *root, const TpString &filePath)
int ConfigJsonParser::writeJsonObjectFileEncryption(TpJsonObject &root, const TpString &filePath)
{
    secret_update_key(); // 更新密钥

    unsigned char *key = secret_get_key();
    if (!key)
    {
        fprintf(stderr, "get key error\n");
        return -1;
    }
    printf("[debug]:secret_get_key ok \n");
    write_json_object_file_key(root, filePath, key);
    return 0;
}

char *ConfigJsonParser::readJsonStrFileEncryption(const TpString &filePath)
{
    unsigned char *key = secret_get_key();
    if (!key)
    {
        fprintf(stderr, "get key error\n");
        return NULL;
    }
    return read_json_string_file_key(filePath, key);
}
