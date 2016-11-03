#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/test.h>
#include <errno.h>

#include <wolfssl/wolfcrypt/sha.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>

// int wc_ShaHash(const byte* data, word32 len, byte* hash)

#define LIMIT 76
#define DIGEST_LEN 20
#define THREAD_NUM 2
#define PASS_LEN 6

const char salt[2] = "Dl";
const char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=";

byte target[DIGEST_LEN];
int alive_count;

void printb(byte *buf, size_t len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02X", buf[i]);
    }
    printf("\n");
}

void printc(byte *buf, size_t len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%c", buf[i]);
    }
    printf("\n");
}

void *brute_force(void *_start)
{
    int start = (int) _start;
    
    Sha sha[1];
    if (wc_InitSha(sha)) {
        WOLFSSL_MSG("wc_InitSha failed");
        alive_count = 0;
        return 0;
    }
    
    
    int end = start + LIMIT/THREAD_NUM;
    byte hash[DIGEST_LEN];
    byte data[PASS_LEN + strlen(salt)];
    memset(hash, 0, sizeof(hash));

    snprintf((char *)data, sizeof(data), "%s", salt);
    int c0, c1, c2, c3, c4, c5;
    for (c0 = start; c0 < end; c0++) {
        data[2] = letters[c0];
        for (c1 = 0; c1 < LIMIT; c1++) {
            printf("searching %c%c****\n", letters[c0], letters[c1]);
            data[3] = letters[c1];
            for (c2 = 0; c2 < LIMIT; c2++) {                
                data[4] = letters[c2];
                for (c3 = 0; c3 < LIMIT; c3++) {
                    data[5] = letters[c3];
                    for (c4 = 0; c4 < LIMIT; c4++) {
                        data[6] = letters[c4];
                        for (c5 = 0; c5 < LIMIT; c5++) {
                            data[7] = letters[c5];                
                            
                            wc_ShaUpdate(sha, data, sizeof(data));
                            wc_ShaFinal(sha, hash);
                            if (!memcmp(target, hash, DIGEST_LEN)) {
                                printf("found key = ");
                                printc(data, sizeof(data));
                                alive_count = 0;
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
                       
    
    
    if (alive_count > 0)
        alive_count--;
    return 0;
}

byte htod(char hex)
{   
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    if (hex >= 'a' && hex <= 'f')
        return hex - 'A' + 10;
    printf("htod failed: character not in range of hex\n");
    return 0;
}

/*
 * Ascii to bytes.
 * Take an string, interpret it as a hex number and store it as bytes.
 * Stored using big-endian representation.
 * str must be TWICE as long as buf, excluding the ending 0.
 */

void atob(byte *buf, char *str, size_t buf_len)
{
    int i, num1, num2;
    for (i = 0; i < buf_len; i++) {
        num1 = htod(str[i * 2]);
        num2 = htod(str[i * 2 + 1]);
        buf[i] = (num1 << 4) + num2;
    }
}


int main()
{
    Sha sha[1];
    if (wc_InitSha(sha)) {
        WOLFSSL_MSG("wc_InitSha failed");
        return 1;
    }

    /*
    byte _target[] = "8647829";
    wc_ShaUpdate(sha, _target, sizeof(_target)-1);
    wc_ShaFinal(sha, target);
    */
    
    // char _target[] = "EC54E814D6023C9FE0BF8020E451C18CA500110D";
    char _target[] = "65CD30D51BF1D197AD61163AC0C3103B4A6D5A20";
    atob(target, _target, sizeof(target));
    printf("target: ");
    printb(target, sizeof(target));

    alive_count = THREAD_NUM;
    pthread_t threads[THREAD_NUM];
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        uintptr_t start = i * LIMIT/THREAD_NUM;
        pthread_create(&threads[i], 0, brute_force, (void *)start);
    }

    while (alive_count) {
        sleep(1);
    }
    
    return 0;
}

