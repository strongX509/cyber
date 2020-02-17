#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

char* copy(char *b)
{
    char  buf_stack[8];
    char *buf_heap = malloc(8);

    strcpy(buf_stack, b);
    strcpy(buf_heap,  b);

    uint64_t canary = *(uint64_t*) (buf_stack +  8);
    uintptr_t rbp   = *(uintptr_t*)(buf_stack + 16);
    uintptr_t rip   = *(uintptr_t*)(buf_stack + 24);

    printf("heap 0x%012" PRIx64 " stack %p cny 0x%016" PRIx64
           " rbp 0x%012" PRIx64 " rip 0x%012" PRIx64 "\n",
           (uintptr_t)buf_heap, buf_stack, canary, rbp, rip);
    return buf_heap;   
}

int main(int argc, char** argv)
{
    char *b;

    if (argc < 2)
    {
        exit(1);
    }
    b = copy(argv[1]);

    free(b);
    exit(0);
}
