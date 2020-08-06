# Floating Point Numbers

## Table of Contents
1. [IEEE 754 Single Precision](#section1)
2. [IEEE 754 Double Precision](#section2)

Python exercises: &nbsp; [PY1](#python1) &nbsp; [PY2](#python2)

## IEEE 754 Single Precision<a name="section1"></a>

The 32-bit binary [Single Precision][IEEE_754_SINGLE] format of the [IEEE 754 Standard for Floating-Point Arithmetic][IEEE_754] consists of a `sign` bit, an 8 bit `exponent` field and a 23 bit `fraction` field:

```  <!-- language: lang-none -->
sign|exponent 8 bits|fraction 23 bits                             |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |S|E|E|E|E|E|E|E|E|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |0              |8              |16             |24           31|
```

The value of the floating point number can be computed from the IEEE 754 representation as follows
```python
number = (1-2*sign) * [1 + fraction*2^-23] * 2^(exponent-127)
```
As you can see the *mantissa* has an additional implicit bit in front of the 23 explicit `fraction` bits.

**Numerical Example**

The following IEEE 754 single precision representation example
```  <!-- language: lang-none -->
sign|exponent 8 bits|fraction 23 bits                             |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |0|0|1|1|1|1|1|0|0|0|1|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |0              |8              |16             |24           31|
```
has a numerical value of
```python
 1 * [1 + (2^21)*(2^-23)] * 2^(0b01111100 - 127) =
 1 * [1 + 2^(21-23)]      * 2^(124-127) =
 1 * [1 + 2^-2]           * 2^-3        = 
 1 * [1 + 0.25]           * 0.125       = 0.15625
```

**Python 1**: <a name="python1"></a>We explore the properties of the C language 32-bit `float` format.

**Positive Powers of Two**

Let's have a look at the representation of some positive powers of two:
```python
>>> from ctypes import *
>>> uint32_p = POINTER(c_uint32)
>>> x = c_float(1)                 # 2^0
>>> print(x)
c_float(1.0)
>>> x_addr = addressof(x)
>>> x32_p = cast(x_addr, uint32_p) # cast float to uint32_t
>>> format(x32_p[0], '032b')
'0 01111111 00000000000000000000000'
>>> x.value = 2.0                  # 2^1
>>> format(x32_p[0], '032b')
'0 10000000 00000000000000000000000'
>>> x.value = 4.0                  # 2^2
>>> format(x32_p[0], '032b')
'0 10000001 00000000000000000000000'
>>> x.value = 65536.0              # 2^16
>>> format(x32_p[0], '032b')
'0 10001111 00000000000000000000000'
>>> x.value = 1 << 127             # 2^127
>>> format(x32_p[0], '032b')
'0 11111110 00000000000000000000000'
>>> x.value = 1 << 128
>>> format(x32_p[0], '032b')       # 2^128
'0 11111111 00000000000000000000000'
>>> print(x)
c_float(inf)                       # positive infinity
```
In the examples above only the `exponent` field is set. The `fraction` field remains unused and the `sign` bit is not set resulting in positive numbers. The largest allowed power of two is `2^127` represented by an exponent field of `11111110` whereas the all ones exponent `11111111` is used to express *positive infinity* (`inf`).

**Negative Numbers**

In the next examples we create some negative numbers which set the `sign`bit to `1`:
```python
>>> x.value = -1                   # -1*2^0
>>> format(x32_p[0], '032b')
'1 01111111 00000000000000000000000'
>>> x.value = -2                   # -1*2^1
>>> format(x32_p[0], '032b')
'1 10000000 00000000000000000000000'
>>> x.value = -4                   # -1*2^2
>>> format(x32_p[0], '032b')
'1 10000001 00000000000000000000000'
>>> x.value = -(1 << 127)          # -1*2^-127
>>> format(x32_p[0], '032b')
'1 11111110 00000000000000000000000'
>>> x.value = -(1 << 128)          # -1*2^128
>>> format(x32_p[0], '032b')
'1 11111111 00000000000000000000000'
>>> print(x)
c_float(-inf)                      # negative infinity
```
An all ones exponent `11111111` together with the sign bit set to `1` is used to express *negative infinity* (`-inf`).

**Negative Powers of Two**

Now we create some negative powers of two which represent positive values smaller than one:
```python
>>> x.value = 0.5                  # 2^-1
>>> format(x32_p[0], '032b')
'0 01111110 00000000000000000000000'
>>> x.value = 0.25                 # 2^-2
>>> format(x32_p[0], '032b')
'0 01111101 00000000000000000000000'
>>> x.value = 0.125
>>> format(x32_p[0], '032b')       # 2^-3
'0 01111100 00000000000000000000000'
>>> x.value = 1.0/(1 << 126)       # 2^-126
>>> format(x32_p[0], '032b')
'0 00000001 00000000000000000000000'
```
Up to a power of `2^-126` the `exponent` field is larger than zero and the mantissa has a *normal* format with an implicit first bit and a full resolution of 23 `fraction` bits.
```python
>>> x.value = 1.0/(1 << 127)       # 2^-127
>>> format(x32_p[0], '032b')
'0 00000000 10000000000000000000000'
>>> x.value = 1/(1 << 149)         # 2^-149
>>> format(x32_p[0], '032b')
'0 00000000 00000000000000000000001'
>>> print(x)
c_float(1.401298464324817e-45)
```
Starting with a power of `2^-127` and up to the minimum representable power of  `2^-149` equivalent to about`10^-45` the numbers are *subnormal* without the implicit first bit and the resolution of the mantissa is reduced.
```python
>>> x.value = 0                    # zero
>>> format(x32_p[0], '032b')
'0 00000000 00000000000000000000000'
```
An all zero exponent and an all zero mantissa represents *zero*.

**Binary Fractions**

The following numbers are composed as a sum of various fractions of two and thus use the `fraction` field:
```python
>>> x.value = 1.5                  # 1 + 2^-1
>>> format(x32_p[0], '032b')
'0 01111111 10000000000000000000000'
>>> x.value = 1.25                 # 1 + 2^-2
>>> format(x32_p[0], '032b')
'0 01111111 01000000000000000000000'
>>> x.value = 1.125                # 1 + 2^-3
>>> format(x32_p[0], '032b')
'0 01111111 00100000000000000000000'
>>> x.value = 1.75                 # 1 + 2^-1 + 2^-2
>>> format(x32_p[0], '032b')
'0 01111111 11000000000000000000000'
>>> x.value = 1.875                # 1 + 2^-1 + 2^-2 + 2^-3
>>> format(x32_p[0], '032b')
'0 01111111 11100000000000000000000'
```
Now we scale all numbers by a factor of two resulting in the same `fraction` value but with an `exponent` increased by one.
```python
>>> x.value = 3                    # (1 + 2^-1) * 2^1
>>> format(x32_p[0], '032b')
'0 10000000 10000000000000000000000'
>>> x.value = 2.5                  # (1 + 2^-2) * 2^1
>>> format(x32_p[0], '032b')
'0 10000000 01000000000000000000000'
>>> x.value = 2.25                 # (1 + 2^-3) * 2^1
>>> format(x32_p[0], '032b')
'0 10000000 00100000000000000000000'
>>> x.value = 3.5                  # (1 + 2^-1 + 2^-2) * 2^1
>>> format(x32_p[0], '032b')
'0 10000000 11000000000000000000000'
>>> x.value = 3.75                 # (1 + 2^-1 + 2^-2 + 2^-3) * 2^1
>>> format(x32_p[0], '032b')
'0 10000000 11100000000000000000000'
```
The biggest number representable by a `float` is  `2^128 - 2^104` and is  about `10^38`:
```python
>>> x.value = 1 << 127
>>> x.value += x.value - (1 << 104)
>>> format(x32_p[0], '032b')
'0 11111110 11111111111111111111111'
>>> print(x)
c_float(3.4028234663852886e+38)
```
By setting the least significant bit of the `fraction` field we see that the maximum resolution of the `float` representation is about 7 decimal digits:
```python
>>> x.value = 1 + 1/(1 << 23)
>>> format(x32_p[0], '032b')
'0 01111111 00000000000000000000001'
>>> print(x)
c_float(1.0000001192092896)
```

**Decimal Fractions**

Decimal fractions cannot be represented exactly because `10 = 2*5` is not a power of `2`
```python
>>> x.value = 1.1
>>> print(x)
c_float(1.100000023841858)
>>> format(x32_p[0], '032b')
'0 01111111 00011001100110011001101'
```
Therefore the evaluation of the discriminant &#916; = `b^2 - 4ac` of a quadratic equation with the coefficients given below does not result in an exact value of `0.0`
```python
>>> a = c_float(0.25)
>>> b = c_float(0.1)
>>> c = c_float(0.01)
>>> print(b.value*b.value - 4*a.value*c.value)
5.21540644005114e-10
```

**Error Handling**

If some internal computing error occurs (e.g. the division 0/0) then the value is set to `NaN` (not a number)
```python
>>> x32_p[0] = 0b01111111110000000000000000000000
>>> format(x32_p[0], '032b')
'0 11111111 10000000000000000000000'
>>> print(x)
c_float(nan)                       # NaN
```
[IEEE_754]: https://en.wikipedia.org/wiki/IEEE_754
[IEEE_754_SINGLE]: https://en.wikipedia.org/wiki/Single-precision_floating-point_format

## IEEE 754 Double Precision<a name="section2"></a>

The 64-bit binary [Double Precision][IEEE_754_DOUBLE] format of the [IEEE 754 Standard for Floating-Point Arithmetic][IEEE_754] consists of a `sign` bit, an 11 bit `exponent` field and a 52 bit `fraction` field:

```  <!-- language: lang-none -->
sign|exponent 11 bits     |fraction 52 bits                                   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-     -+-+-+-+-+-+-+-+-+-+
  |S|E|E|E|E|E|E|E|E|E|E|E|F|F|F|F|F|F|F|F|F|F|F|F|F|F ... F|F|F|F|F|F|F|F|F|F|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-     -+-+-+-+-+-+-+-+-+-+
  |0              |8              |16             |24         |56           63|
```

Again by completing the *mantissa* with an additional implicit bit in front of the 53 `fraction` bits, the value of the floating point number can be computed from the IEEE 754 representation as follows

```python
number = (1-2*sign) * [1 + fraction*2^-52] * 2^(exponent-1023)
```

**Python 2**: <a name="python2"></a>We explore the properties of the C language 64-bit `double` format.

**Positive Powers of Two**

```python
>>> from ctypes import *
>>> uint64_p = POINTER(c_uint64)
>>> x = c_double(1)
>>> print(x)
c_double(1.0)
>>> x_addr = addressof(x)
>>> x64_p = cast(x_addr, uint64_p)
>>> format(x64_p[0], '064b')
'0 01111111111 0000000000000000000000000000000000000000000000000000'
>>> x.value = 2
>>> format(x64_p[0], '064b')
'0 10000000000 0000000000000000000000000000000000000000000000000000'
>>> x.value = 65536
>>> format(x64_p[0], '064b')
'0 10000001111 0000000000000000000000000000000000000000000000000000'
>>> x.value = 1 << 1023
>>> format(x64_p[0], '064b')
'0 11111111110 0000000000000000000000000000000000000000000000000000'
>>> x.value += x.value
>>> format(x64_p[0], '064b')
'0 11111111111 0000000000000000000000000000000000000000000000000000'
>>> print(x)
c_double(inf)
```

**Negative Powers of Two**

Now we create some negative powers of two which represent values smaller than one:
```python
>>> x.value = 0.5                  # 2^-1
>>> format(x64_p[0], '064b')
'0 01111111110 0000000000000000000000000000000000000000000000000000'
>>> x.value = 0.25                 # 2^-2
>>> format(x64_p[0], '064b')
'0 01111111101 0000000000000000000000000000000000000000000000000000'
>>> x.value = 0.125                # 2^-3
>>> format(x64_p[0], '064b')
'0 01111111100 0000000000000000000000000000000000000000000000000000'
>>> x.value = 1/(1 << 1022)        # 2^-1022
>>> format(x64_p[0], '064b')
'0 00000000001 0000000000000000000000000000000000000000000000000000'
```
Up to a power of `2^-1022` the `exponent` field is larger than zero and the mantissa has a *normal* format with an implicit first bit and a full resolution of 52 `fraction` bits.
```python
>>> x.value = 1/(1 << 1023)        # 2^-1023
>>> format(x64_p[0], '064b')
'0 00000000000 1000000000000000000000000000000000000000000000000000'
>>> x.value = 1/(1 << 1074)        # 2^-1074
>>> format(x64_p[0], '064b')
'0 00000000000 0000000000000000000000000000000000000000000000000001'
>>> print(x)
c_double(5e-324)
```
Starting with a power of `2^-1023` and up to the minimum representable power of `2^-1047` equivalent to about`10^-324` the numbers are *subnormal* without the implicit first bit and the resolution of the mantissa is reduced.
```python
>>> x.value = 0                    # zero
>>> format(x64_p[0], '064b')
'0 00000000000 0000000000000000000000000000000000000000000000000000'
```
An all zero exponent and an all zero mantissa represents *zero*.

**Binary Fractions**

The biggest number representable by a `double` is  `2^1024 - 2^971` and is  about `10^308`:

```python
>>> x.value = 1 << 1023
>>> x.value += x.value - (1 << 971)
>>> format(x64_p[0], '064b')
'0 11111111110 1111111111111111111111111111111111111111111111111111'
>>> print(x)
c_double(1.7976931348623157e+308)
```
By setting the least significant bit of the `fraction` field we see that the maximum resolution of the `double` representation is about 16 decimal digits:
```python
>>> x.value = 1 + 1/(1 << 52)
>>> format(u64_p[0], '064b')
'0 01111111111 0000000000000000000000000000000000000000000000000001'
>>> print(x)
c_double(1.0000000000000002)
```

**Decimal Fractions**

Decimal fractions cannot be represented exactly because `10 = 2*5` is not a power of `2`
```python
>>> x.value = 1.1
>>> print(x)
c_double(1.1)
>>> format(x64_p[0], '064b')
'0 01111111111 0001100110011001100110011001100110011001100110011010'
```
Therefore the evaluation of the discriminant &#916; = `b^2 - 4ac` of a quadratic equation with the coefficients given below does not result in an exact value of `0.0`
```python
>>> a = c_double(0.25)
>>> b = c_double(0.1)
>>> c = c_double(0.01)
>>> print(b.value*b.value - 4*a.value*c.value)
1.734723475976807e-18
```

[IEEE_754_DOUBLE]: https://en.wikipedia.org/wiki/Double-precision_floating-point_format

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
