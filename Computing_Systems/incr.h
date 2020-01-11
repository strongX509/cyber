#ifndef INCR_H_
#define INCR_H_

#include <stdint.h>

/**
 * Increments each of the inputs v64, v32, v16, and v8 by one and returns
 * the results in host order packed into buf
 */
void incr(uint64_t v64, uint32_t v32, uint16_t v16, uint8_t v8, uint8_t buf[]);

#endif /** INCR_H_ */
