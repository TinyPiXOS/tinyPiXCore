#pragma once

#include <openssl/evp.h>

#ifdef __cplusplus
extern "C" {
#endif

#if OPENSSL_VERSION_NUMBER < 0x30000000L
int EVP_CIPHER_CTX_get_block_size(const EVP_CIPHER_CTX *ctx);
#endif

#ifdef __cplusplus
}
#endif
