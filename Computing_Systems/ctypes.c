#include <stdlib.h>
#include <stdio.h>

#include "incr.h"

int main(int argc, char** argv)
{
    uint64_t v64 = 0x0011223344556677;
    uint32_t v32 = 0x8899aabb;
    uint16_t v16 = 0xccdd;
    uint8_t  v8  = 0xee;
    uint8_t buf[16];
    const int len = sizeof(uint64_t) + sizeof(uint32_t) +
                    sizeof(uint16_t) + sizeof(uint8_t);
    int i;

    incr(v64, v32, v16, v8, buf);

    printf("buf = 0x");
    for (i = 0; i < len; i++)
    {
        printf("%02x", buf[i]);
    }
    printf("\n");
    exit(0);
}
