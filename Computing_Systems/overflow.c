#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

void copy(char *b)
{
    char  buf[8];

    strcpy(buf, b);

    printf("&buf 0x%016" PRIx64 " rbp 0x%016" PRIx64 " rip 0x%016" PRIx64 "\n",
          (intptr_t*)buf, *(intptr_t*)(buf + 8), *(intptr_t*)(buf + 16));
}

int main(int argc, char** argv)
{
    char *b;

    if (argc < 2)
    {
        exit(1);
    }
    copy(argv[1]);
    printf("We happily returned!\n");
    exit(0);
}
