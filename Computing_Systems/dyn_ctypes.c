#include <stdlib.h>
#include <stdio.h>

#include "dyn_incr.h"

/* global uninitalized variables */
uint32_t buf[16];

/* global initialized variables */
uint64_t v64 = 0x0011223344556677;
uint32_t v32 = 0x8899aabb;

/* global constants */
const uint16_t v16 = 0xccdd;
const uint8_t  v8  = 0xee;

int main(int argc, char** argv)
{
    uint8_t *buf1, *buf2;
    const int len = sizeof(uint64_t) + sizeof(uint32_t) +
                    sizeof(uint16_t) + sizeof(uint8_t);
    int i;

    buf1 = incr(v64, v32, v16, v8, 1);
	buf2 = incr(v64, v32, v16, v8, 2);

    printf("buf1 = 0x");
    for (i = 0; i < len; i++)
    {
        printf("%02x", buf1[i]);
    }
    printf("\n");

    printf("buf2 = 0x");
    for (i = 0; i < len; i++)
    {
        printf("%02x", buf2[i]);
    }
    printf("\n");

    free(buf1);
	free(buf2);
    exit(0);
}
