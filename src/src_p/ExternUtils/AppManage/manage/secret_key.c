/*///------------------------------------------------------------------------------------------------------------------------//
		密钥管理
说 明 : 
日 期 : 2025.6.7

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include "secret_key.h"
#include "utilslib.h"



#define KEY_FILE_PATH	"/etc/tpssl/tpappkey.bin"
#define PASSPHRASE 		"tpAppManagerBinPassphrase"

const char *UserPassphrase=PASSPHRASE;

// 生成随机密钥
void generate_key(unsigned char* key) {
    RAND_bytes(key, KEY_LEN);  // 使用OpenSSL安全随机源[9](@ref)
}

// 口令派生密钥（PBKDF2算法）
void derive_key(const char* passphrase, unsigned char* derived_key, const unsigned char* salt) 
{
    PKCS5_PBKDF2_HMAC(passphrase, strlen(passphrase),
                      salt, SALT_LEN, 
                      100000,  // 迭代次数增强安全性
                      EVP_sha256(), 
                      KEY_LEN, derived_key);
}

// 加密并存储密钥
//file_path:保存路径
//raw_key:密钥
//passphrase:用户口令
int store_encrypted_key(const char* file_path, const unsigned char* raw_key,const char* passphrase) {
    unsigned char salt[SALT_LEN];
    if (RAND_bytes(salt, SALT_LEN) != 1) {
        fprintf(stderr, "盐值生成失败\n");
        return -1;
    }

    unsigned char derived_key[KEY_LEN];
    derive_key(passphrase, derived_key, salt);

    unsigned char iv[AES_BLOCK_SIZE];
    if (RAND_bytes(iv, AES_BLOCK_SIZE) != 1) {
        fprintf(stderr, "IV生成失败\n");
        return -1;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -1;

    // 核心修复：正确处理加密长度
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, derived_key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    int cipher_len = KEY_LEN + EVP_CIPHER_CTX_block_size(ctx);
    unsigned char* ciphertext = malloc(cipher_len);
    int out_len1, out_len2;

    // 加密操作
    if (EVP_EncryptUpdate(ctx, ciphertext, &out_len1, raw_key, KEY_LEN) != 1 ||
        EVP_EncryptFinal_ex(ctx, ciphertext + out_len1, &out_len2) != 1) 
    {
        free(ciphertext);
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    int total_cipher_len = out_len1 + out_len2;

    // 写入文件
    FILE* fp = safe_fopen(file_path, "wb");
    if (!fp) {
        free(ciphertext);
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    fwrite(salt, 1, SALT_LEN, fp);
    fwrite(iv, 1, AES_BLOCK_SIZE, fp);
    fwrite(ciphertext, 1, total_cipher_len, fp);
    
    // 清理资源
    fclose(fp);
    free(ciphertext);
    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_cleanse(derived_key, KEY_LEN);
    return 0;
}

//从文件读取密钥
unsigned char* load_decrypted_key(const char* file_path, const char* passphrase) 
{
    FILE* fp = fopen(file_path, "rb");
    if (!fp) return NULL;

    // 读取盐值和IV
    unsigned char salt[SALT_LEN], iv[AES_BLOCK_SIZE];
    if (fread(salt, 1, SALT_LEN, fp) != SALT_LEN ||
        fread(iv, 1, AES_BLOCK_SIZE, fp) != AES_BLOCK_SIZE) 
    {
        fclose(fp);
        return NULL;
    }

    // 获取密文长度
    fseek(fp, 0, SEEK_END);
    long cipher_len = ftell(fp) - SALT_LEN - AES_BLOCK_SIZE;
    fseek(fp, SALT_LEN + AES_BLOCK_SIZE, SEEK_SET);

    unsigned char* ciphertext = malloc(cipher_len);
    if (fread(ciphertext, 1, cipher_len, fp) != cipher_len) {
        free(ciphertext);
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    // 口令派生密钥
    unsigned char derived_key[KEY_LEN];
    derive_key(passphrase, derived_key, salt);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        free(ciphertext);
        return NULL;
    }

    // 核心修复：正确处理解密长度
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, derived_key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(ciphertext);
        return NULL;
    }

    unsigned char* plaintext = malloc(cipher_len);
    int out_len1, out_len2;

    // 解密操作
    if (EVP_DecryptUpdate(ctx, plaintext, &out_len1, ciphertext, cipher_len) != 1 ||
        EVP_DecryptFinal_ex(ctx, plaintext + out_len1, &out_len2) != 1) 
    {
        free(plaintext);
        free(ciphertext);
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    int total_plain_len = out_len1 + out_len2;

    // 验证密钥长度
    if (total_plain_len != KEY_LEN) {
        fprintf(stderr, "警告：解密密钥长度异常(%d != %d)\n", total_plain_len, KEY_LEN);
    }

    // 清理资源
    free(ciphertext);
    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_cleanse(derived_key, KEY_LEN);
    return plaintext;
}

//更新密钥
int secret_update_key()
{
	//生成密钥
	unsigned char raw_key[KEY_LEN];
    generate_key(raw_key);
    printf("生成的原始密钥: ");
    for (int i = 0; i < KEY_LEN; i++) 
		printf("%02x", raw_key[i]);
    printf("\n");

    //加密存储密钥
	if (store_encrypted_key(KEY_FILE_PATH, raw_key, UserPassphrase) != 0) {
        fprintf(stderr, "密钥存储失败\n");
        return -1;
    }
    
    // 立即验证密钥
    unsigned char* decrypted = load_decrypted_key(KEY_FILE_PATH, UserPassphrase);
    if (!decrypted) {
        fprintf(stderr, "密钥验证失败：无法读取\n");
        return -1;
    }
    
    printf("解密验证密钥: ");
    for (int i = 0; i < KEY_LEN; i++) printf("%02x", decrypted[i]);
    printf("\n");
    
    // 对比密钥
    if (memcmp(raw_key, decrypted, KEY_LEN) != 0) {
        fprintf(stderr, "严重错误：密钥不匹配！\n");
        free(decrypted);
        return -1;
    }
    
    free(decrypted);
    OPENSSL_cleanse(raw_key, KEY_LEN);

	printf("密钥已加密存储至: %s\n", KEY_FILE_PATH);
    return 0;
}

//获取密钥
unsigned char *secret_get_key()
{
	return load_decrypted_key(KEY_FILE_PATH, UserPassphrase);
}