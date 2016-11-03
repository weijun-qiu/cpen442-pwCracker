#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/test.h>
#include <errno.h>

#include <wolfssl/wolfcrypt/sha.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>

// int wc_ShaHash(const byte* data, word32 len, byte* hash)

#define LIMIT 100000000
#define DIGEST_LEN 20
#define THREAD_NUM 2
#define PASS_LEN 8

// const char salt[2] = "nk";
const char salt[2] = "";

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

    int trial = start;
    int end = start + LIMIT/THREAD_NUM + THREAD_NUM;
    byte hash[DIGEST_LEN];
    byte data[PASS_LEN + strlen(salt) + 1];
    memset(hash, 0, sizeof(hash));
    while (trial < end) {
        snprintf((char *)data, sizeof(data), "%s%08d", salt, trial);
        printc(data, sizeof(data));
        wc_ShaUpdate(sha, data, sizeof(data) - 1);
        wc_ShaFinal(sha, hash);
        if (!memcmp(target, hash, DIGEST_LEN)) {
            printf("found key = %d\n",trial);
            alive_count = 0;
            return 0;
        }
        trial++;
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
    
    // char _target[] = "C79A7C46DCE011C8488E096A1ED89771AD244CC4";
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

