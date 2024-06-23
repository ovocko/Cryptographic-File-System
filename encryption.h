#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <openssl/evp.h>

void init_openssl();
void cleanup_openssl();
void handleErrors();
extern void set_key_iv(const char *key, const char *iv);  // Declaration of set_key_iv function
void encrypt(const char *input, char *output, int *output_len);
void decrypt(const char *input, char *output, int input_len, int *output_len);

extern unsigned char key[32];
extern unsigned char iv[16];

#endif // ENCRYPTION_H

