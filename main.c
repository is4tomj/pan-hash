#define _OPEN_SYS_ITOA_EXT
#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <string.h>

void sha256(char *string, char outputBuffer[65])
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(hash, &sha256);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

#define PAN_CARDSIZE 16
#define PAN_HASHSIZE 64
#define PAN_BUFFSIZE 256*(PAN_CARDSIZE+PAN_HASHSIZE)

int main(int argv, char** argc)
{

    char buff[PAN_BUFFSIZE]; // this is enough for 256 card numbers
    unsigned int offset = 0;
    
    //printf("hello world: %s\n", hash);
    // 16 numbers, six being fixed and 1 being a luhn, means only 9 values
    const unsigned long long init = 5191230000000000;
    unsigned long long i = init;
    const unsigned long long max = init+ 10000000000;
    const int parity = 16 % 2;

    char panChars[PAN_CARDSIZE+1];

    for(; i<max; i+=10)
    {
        //printf("i:%llu\n", i);
        // compute luhn
        unsigned long long luhn = 0;
        unsigned long long val = i;
        unsigned long long panNum = 0;
        unsigned long long digit;
        for(int j=1; j<16; j++)
        {
            val /= 10;
            digit = val%10;

            if((j-1)%2 == parity) // if odd
            {
                digit = digit * 2;
                luhn += digit/10;
            }
            luhn += digit%10;
        }
        luhn = (luhn*9) % 10;
        panNum = ((i/10)*10) + luhn;


        sprintf(panChars, "%llu", panNum); // this creats a C-string, which needs an extra byte for null
        memcpy(buff+offset, panChars, PAN_CARDSIZE);
        offset += PAN_CARDSIZE;

        char hash[256];
        sha256(panChars, hash);
        memcpy(buff+offset, hash, PAN_HASHSIZE);
        offset += PAN_HASHSIZE;

        if(offset == PAN_BUFFSIZE)
        {
            unsigned long long total = max - init;
            unsigned long long done = total - (max-i);
            fprintf(stderr, "\r%llu%%, i:%llu, num to go:%llu", (done*100)/(total), i, max-i);

            fwrite(buff, offset, 1, stdout);
            offset = 0;
        }

    }
    // flush buffer to stdout
    fwrite(buff, offset, 1, stdout);
    fprintf(stderr, "100%%");

            
    printf("\n");
    return 0;
}
