#include <stdlib.h>
#include <arpa/inet.h>

#include "dyn_incr.h"

uint8_t* incr(uint64_t v64, uint32_t v32, uint16_t v16, uint8_t v8, uint8_t n)
{
    uint8_t *buf = (uint8_t*)malloc(16);

    /* increment inputs by n */
    v64 += n;
    v32 += n;
    v16 += n;
    v8  += n;

    /* cast results in network order into buffer */
    *((uint32_t*)&buf[0])  = htonl(v64 >> 32);
    *((uint32_t*)&buf[4])  = htonl(v64 & 0x0000000ffffffff);
    *((uint32_t*)&buf[8])  = htonl(v32);
    *((uint16_t*)&buf[12]) = htons(v16);
    *((uint8_t*) &buf[14]) = v8;

    return buf;
}

