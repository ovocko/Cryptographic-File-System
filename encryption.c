#include "encryption.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

unsigned char key[32]; // Global variables for key and IV
unsigned char iv[16];

void init_openssl() {
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
    ERR_free_strings();
}

void handleErrors(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

void set_key_iv(const char *key_str, const char *iv_str) {
    strncpy((char *)key, key_str, sizeof(key));
    strncpy((char *)iv, iv_str, sizeof(iv));
}

void encrypt(const char *input, char *output, int *output_len) {
    EVP_CIPHER_CTX *ctx;
    int len;

    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_EncryptUpdate(ctx, (unsigned char *)output, &len, (unsigned char *)input, strlen(input)))
        handleErrors();
    *output_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, (unsigned char *)output + len, &len)) handleErrors();
    *output_len += len;

    EVP_CIPHER_CTX_free(ctx);
}

void decrypt(const char *input, char *output, int input_len, int *output_len) {
    EVP_CIPHER_CTX *ctx;
    int len;

    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_DecryptUpdate(ctx, (unsigned char *)output, &len, (unsigned char *)input, input_len))
        handleErrors();
    *output_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, (unsigned char *)output + len, &len)) handleErrors();
    *output_len += len;

    EVP_CIPHER_CTX_free(ctx);
}

