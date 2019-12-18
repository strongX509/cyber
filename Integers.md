# Integers

## Table of Contents
1. [Integer Types](#section1)
2. [Endianness](#section2)
3. [Two's Complement](#section3)

## Integer Types <a name="section1"></a>

The following portable integer types are defined by the [stdint.h][STDINT] header file standardized by  *POSIX.1-2017* which is equivalent to *IEEE Std 1003.1-2017* and *The Open Group Base Specifications Issue 7, 2018 edition*. 

| Std Type     | C Type             | Bits |                   Range  (MIN .. MAX)                   |
| :----------- | :----------------- | :--: | :-----------------------------------------------------: |
| **uint8_t**  | unsigned char      |  8   |                        0 .. 255                         |
| **int8_t**   | char               |  8   |                       -128 .. 127                       |
| **uint16_t** | unsigned short/int |  16  |                       0 .. 65'535                       |
| **int16_t**  | short/int          |  16  |                    -32'768 .. 32'767                    |
| **uint32_t** | unsigned int/long  |  32  |                   0 .. 4'294'967'295                    |
| **int32_t**  | int/long           |  32  |             -2'147'483'648 .. 2'147'483'647             |
| **uint64_t** | unsigned long long |  64  |             0 .. 18'446'744'073'709'551'615             |
| **int64_t**  | long long          |  64  | -9'223'372'036'854'775'808 .. 9'223'372'036'854'775'807 |

The size of the C types `unsigned int`, `int`, `unsigned long` and `long` depends on the processor architecture (16, 32 or 64 bit). Therefore if exact size is an issue, the use of the standard types is strongly recommended.

**Python**: In the following  examples we explore the cyclic overflow behaviour of the standard **unsigned** integer types by using the Python `ctypes` library.

We cause an overflow by adding `1` to an `uint8_t` set to `UINT8_MAX`
```python
>>> from ctypes import *
>>> x = c_uint8(255)         # uint8_t set to UINT8_MAX
>>> print(x)
c_ubyte(255)
>>> x.value += 1
>>> print(x)
c_ubyte(0)
```
We cause an underflow by subtracting `1` from an `uint16_t` set to `0`
```python
>>> from ctypes import *
>>> x = c_uint16()          # uint16_t
>>> print(x)
c_ushort(0)                 # uint16_t maps to an unsigned short
>>> x.value -= 1
>>> print(x)
c_ushort(65535)
>>> format(x.value, '#06x')
'0xffff'
```
We cause an overflow by adding `10` to an `uint32_t` set to `UINT32_MAX`
```python
>>> from ctypes import *
>>> x = c_uint32(0xffffffff)  # uint32_t set to UINT32_MAX
>>> print(x)
c_uint(4294967295)            # uint32_t maps to an unsigned int
>>> x.value += 10
>>> print(x)
c_uint(9)
```
We cause an overflow by squaring an `uint64_t` set to `2^32`
```python
>>> from ctypes import *
>>> x = c_uint64(0x100000000) # uint64_t
>>> print(x)
c_ulong(4294967296)           # uint64_t maps to an unsigned long
>>> x.value *= x.value
>>> print(x)
c_ulong(0)
```

[STDINT]: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/stdint.h.html

## Endianness <a name="section2"></a>

## Two's Complement <a name="section3"></a>

![Cyclic Addition](Twos_Complement_400.png "Cyclic Addition")

**Inversion Rule**: `-x = ~x + 1` The negative `-x` of a number `x` in binary two's complement representation is computed by *bitwise* inversion of `x` and then adding `1` to the inverted value.

Thus the bit inversion of  `0010` (+2)  results in `1101` (-3) and by adding `1` gives the final value of  `1110` (-2). A special case is zero with its representation of `0000` that gets inverted to `1111` (-1) and by adding `1` becomes `0000` again.

Graphically the negation of a number can be interpreted as mirroring the circle position of `x` on the horizontal axis (thick blue line in the picture above) and then rotating the flipped value `~x` by one position on the circle counter-clockwise.

Generally *addition* by one is equivalent to a counter-clockwise rotation of the number position on the circle whereas *subtraction* by one translates into a clockwise rotation. 

When the largest representable *positive* value is exceeded during addition, an *overflow* occurs and the result takes on the largest representable *negative* value. E.g. adding `1` to  `0111` (+7) results in `1000` (-8).

**Python:** In the following  examples we explore the cyclic overflow behaviour of the standard **signed** integer types by using the Python `ctypes` library.

We cause an overflow by adding `1` to an `int8_t` set to `INT8_MAX`
```python
>>> from ctypes import *
>>> x = c_int8(0b01111111)       # int8_t set to INT8_MAX
>>> print(x)
c_byte(127)
>>> x.value += 1
>>> print(x)
c_byte(-128)
```
We cause an underflow by subtracting `1` from an `int16_t` set to `INT16_MIN`
```python
>>> from ctypes import *
>>> x = c_int16(0x8000)          # int16_t set to INT16_MIN
>>> print(x)
c_short(-32768)                  # int16_t maps to a short
>>> x.value -= 1
>>> print(x)
c_short(32767)
```
We  add an`int32_t` set to `INT32_MAX` and an `int32_t` set to `INT32_MIN` to each other
```python
>>> from ctypes import *
>>> x_max = c_int32(0x7fffffff)  # int32_t set to INT32_MAX
>>> x_min = c_int32(0x80000000)  # int32_t set to INT32_MIN
>>> print(x_max)
c_int(2147483647)                # int32_t maps to an int
>>> print(x_min)
c_int(-2147483648)
>>> y = c_int32()
>>> y.value = x_max.value + x_min.value
>>> print(y)
c_int(-1)
```
We add `INT64_MIN = -2^63` to an `int64_t`value set to `2^62`
```python
>>> from ctypes import *
>>> x = c_int64(1 << 62)         # int64_t
>>> print(x)
c_long(4611686018427387904)      # int64_t maps to a long
>>> x.value += (1 << 63)
>>> print(x)
c_long(-4611686018427387904)
```

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
