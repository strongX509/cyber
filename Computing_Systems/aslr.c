#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

char* copy(char *b)
{
    char  buf_stack[8];
    char *buf_heap = malloc(8);

    uint64_t canary = *(uint64_t*)(buf_stack +  8);
    intptr_t rbp    = *(intptr_t*)(buf_stack + 16);
    intptr_t rip    = *(intptr_t*)(buf_stack + 24);

    strcpy(buf_stack, b);
    strcpy(buf_heap,  b);

    printf("heap %p stack %p cny 0x%016" PRIx64 " rbp %p rip %p\n",
            buf_heap, buf_stack, canary, (void*)rbp, (void*)rip);

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
