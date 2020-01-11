#include "incr.h"

void incr(uint64_t v64, uint32_t v32, uint16_t v16, uint8_t v8, uint8_t buf[])
{
    /* increment inputs by one */
    v64 += 1;
    v32 += 1;
    v16 += 1;
    v8  += 1;

    /* cast results into buffer */
    *((uint64_t*)&buf[0]) = v64;
    *((uint32_t*)&buf[8]) = v32;
    *((uint16_t*)&buf[12]) = v16;
    *((uint8_t*) &buf[14]) = v8;
}

