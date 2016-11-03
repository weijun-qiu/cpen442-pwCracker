#include <wolfssl/wolfcrypt/sha.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define DIGEST_SIZE 20

int main(int argc, char **argv)
{       
    /* Check arguments */
    if (argc != 2) {
        puts("Usage: pwSetter newpassword");
        return 1;
    }

    /* Initialize crypto library */
    Sha sha[1];
    if (wc_InitSha(sha)) {
        puts("wc_InitSha failed");
        return 1;
    }
    
    /* Compute hash of password */
    char *pw = argv[1];
    const size_t pw_len = strlen(pw);
    byte digest[DIGEST_SIZE];
    
    wc_ShaUpdate(sha, (byte *)pw, pw_len);
    wc_ShaFinal(sha, digest);

    /* Prints */
    /*
    printf("new password: %s\n", pw);
    printf("new hash: ");
    int i;
    for (i = 0; i < 20; i++) {
        printf("%02X", hash[i]);
    }
    printf("\n");
    */

    /* Modify program2.exe */
    FILE *fp;
    if (!(fp = fopen("42701136.program2.exe", "r+"))) {
        perror("fopen failed");
        return 1;
    }    
    if (fseek(fp, 0x01E012, SEEK_SET)) {
        perror("fseek failed");
        return 1;
    }    
    if ((fwrite(digest, 1, DIGEST_SIZE, fp)) != DIGEST_SIZE) {
        perror("fwrite failed");
        return 1;
    }
    fclose(fp);   
    return 0;
}
    
