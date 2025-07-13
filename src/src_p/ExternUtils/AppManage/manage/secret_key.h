#ifndef _SECRET_KEY_H_
#define _SECRET_KEY_H_

#define KEY_LEN 32  // AES-256密钥长度
#define SALT_LEN 16
extern const char *UserPassphrase;

int secret_update_key();
unsigned char *secret_get_key();

#endif
