#ifndef DYN_INCR_H_
#define DYN_INCR_H_

#include <stdint.h>

/**
 * Increments each of the inputs v64, v32, v16, and v8 by n and returns
 * the results in a network order packed byte array
 */
uint8_t* incr(uint64_t v64, uint32_t v32, uint16_t v16, uint8_t v8, uint8_t n);

#endif /** DYN_INCR_H_ */
