# Number Systems

## Table of Contents
1. [Representation of Natural Numbers](#section1)
2. [Positional Numeral Systems](#section2)
3. [Bits, Bytes and Words](#section3)
4. [Base Conversions](#section4)

Python exercises: &nbsp; [PY1](#python1) &nbsp; [PY2](#python2)

## Representation of Natural Numbers <a name="section1"></a>

The [natural numbers][NAT_NUM] consist of all positive integers 1, 2, 3, ... Often zero is also included, so that the natural numbers cover all non-negative integers 0, 1, 2, 3, ...

Natural numbers are usually represented in the [decimal numeral system][DEC_NUM] or base-10 [positional numeral system][POS_NUM]:
```
703 = 7*10^2 + 0*10^1 + 3*10^0
    = 7*100  + 0*10   + 3*1
```
But besides the human possessing 10 fingers on his two hands there is no special advantage in using 10 as a base for a [positional numeral system][POS_NUM]. The generic positional notation for a number with an arbitrary base or radix is shown below.

![Number = Sum(Digit * Radix^Index)][POS_NUM_IMG]
```
Number = ... + Digit*Radix^Index + ... + Digit_1*Radix^1 + Digit_0*Radix^0
```
Actually for a computer built from electronic NAND and NOR gates implementing Boolean logic, the base-2 [binary numeral system][BIN_NUM] is a much more convenient number representation:
```
1010111111 = 1*2^9 + 0*2^8 + 1*2^7 + 0*2^6 + 1*2^5 + 1*2^4 + 1*2^3 + 1*2^2 + 1*2^1 + 1*2^0
             1*512 + 0*256 + 1*128 + 0*64  + 1*32  + 1*16  + 1*8   + 1*4   + 1*2   + 1*1
```
For humans the many digits of a binary number representation are quite tiresome. Therefore the more compact base-8 [octal numeral system][OCT_NUM]
```
1277 = 1*8^3 + 2*8^2 + 7*8^1 + 7*8^0
     = 1*512 + 2*64  + 7*8   + 7*1
```
or the very popular base-16 [hexadecimal numeral system][HEX_NUM]
```
2BF = 2*16^2 + B*16^1 + F*16^0
    = 2*256  + 11*16  + 15*1
```
are extensively used in computer and software engineering.

[NAT_NUM]: https://en.wikipedia.org/wiki/Natural_number
[POS_NUM]: https://en.wikipedia.org/wiki/Positional_notation
[DEC_NUM]: https://en.wikipedia.org/wiki/Decimal
[BIN_NUM]: https://en.wikipedia.org/wiki/Binary_number
[OCT_NUM]: https://en.wikipedia.org/wiki/Octal
[HEX_NUM]: https://en.wikipedia.org/wiki/Hexadecimal
[POS_NUM_IMG]: https://upload.wikimedia.org/wikipedia/commons/thumb/7/78/Positional_notation_glossary-en.svg/500px-Positional_notation_glossary-en.svg.png  "Terms used in positional numeral systems, Michel Bakni CC BY-SA"

## Positional Numeral Systems <a name="section2"></a>

The following table shows the symbols used by the hexadecimal, decimal, octal and binary numeral systems to encode the first 16 natural numbers including zero.

| Hex | Decimal | Octal | Binary |
|:---:|:-------:|:-----:|-------:|
|**0**|     0   |   0   |     0  |
|**1**|     1   |   1   |     1  |
|**2**|     2   |   2   |    10  |
|**3**|     3   |   3   |    11  |
|**4**|     4   |   4   |   100  |
|**5**|     5   |   5   |   101  |
|**6**|     6   |   6   |   110  |
|**7**|     7   |   7   |   111  |
|**8**|     8   |  10   |  1000  |
|**9**|     9   |  11   |  1001  |
|**A**|    10   |  12   |  1010  |
|**B**|    11   |  13   |  1011  |
|**C**|    12   |  14   |  1100  |
|**D**|    13   |  15   |  1101  |
|**E**|    14   |  16   |  1110  |
|**F**|    15   |  17   |  1111  |

The next table lists the power of each digit position for the decimal, binary, octal and hexadecimal numeral systems.

|   n  |    10^n |Units|   k  |      2^k |&#948;|  i   |       8^i |   m  |      16^m |
|-----:|--------:|-----|-----:|---------:|-----:|-----:|----------:|-----:|----------:|
| **0**|        1|     | **0**|        1 |      | **0**|         1 | **0**|         1 |
|      |         |     | **1**|        2 |      |   -  |           |   -  |           |
|      |         |     | **2**|        4 |      |   -  |           |   -  |           |
|      |         |     | **3**|        8 |      | **1**|         8 |   -  |           |
| **1**|       10|     |   -  |          |      |   -  |           |   -  |           |
|      |         |     | **4**|       16 |      |   -  |           | **1**|        16 |
|      |         |     | **5**|       32 |      |   -  |           |   -  |           |
|      |         |     | **6**|       64 |      | **2**|        64 |   -  |           |
| **2**|      100|     |   -  |          |      |   -  |           |   -  |           |
|      |         |     | **7**|      128 |      |   -  |           |   -  |           |
|      |         |     | **8**|      256 |      |   -  |           | **2**|       256 |
|      |         |     | **9**|      512 |      | **3**|       512 |   -  |           |
| **3**|     1000|kilo |**10**|    1'024 |  2.4%|   -  |           |   -  |           |
|      |         |     |**11**|    2'048 |      |   -  |           |   -  |           |
|      |         |     |**12**|    4'096 |      | **4**|     4'096 | **3**|     4'096 |
|      |         |     |**13**|    8'192 |      |   -  |           |   -  |           |
|      |         |     |**14**|   16'384 |      |   -  |           |   -  |           |
|      |         |     |**15**|   32'768 |      | **5**|    32'768 |   -  |           |
|      |         |     |**16**|   65'536 |      |   -  |           | **4**|    65'536 |
| **6**|1'000'000|Mega |**20**|1'048'576 |  4.9%|   -  |           | **5**| 1'048'576 |
|      |         |     |**24**|&#8776;16M|      | **8**|&#8776;16M | **6**|&#8776;16M |
| **9**|       1G|Giga |**30**|&#8776;1G |  7.4%|**10**|&#8776;1G  |   -  |           |
|      |         |     |**32**|&#8776;4G |      |   -  |           | **8**|&#8776;4G  |
|**12**|       1T|Tera |**40**|&#8776;1T | 10.0%|   -  |           |**10**|&#8776;1T  |
|      |         |     |**48**|          |      |**16**|&#8776;256T|**12**|&#8776;256T|
|**15**|       1P|Peta |**50**|&#8776;1P | 12.6%|   -  |           |   -  |           |
|**18**|       1E|Exa  |**60**|&#8776;1E | 15.3%|**20**|&#8776;1E  |**15**|&#8776;1E  |
|      |         |     |**64**|&#8776;16E|      |   -  |           |**16**|&#8776;16E |
|**21**|       1Z|Zetta|**70**|&#8776;1Z | 18.1%|   -  |           |   -  |           |
|**24**|       1Y|Yotta|**80**|&#8776;1Y | 20.9%|   -  |           |**20**|&#8776;1Y  |

Thus a 32 bit word encodes `2^32 = 4'294'967'296` or approximately 4G different values whereas a 64 bit word represents `2^64 = 18'446'744'073'709'551'616` or about 16E different numbers.

The address space of an `x86-64`64 bit processor is currently 48 bits allowing it to address `2^48 = 281'474'976'710'656` bytes amounting to about 256TB of memory.

The relative deviation &#948; between the scaling factors in the decimal and  binary number systems is given by  `2^k/10^n - 1` = `1.024^(n/3) - 1` and according to the table above amounts to 2.4% for`kilo` and 20.9% for `Yotta`. Transmission speeds like e.g. 64 kbit/s = 64'000 bits/s use decimal scaling factors whereas storage capacities like 64 kbytes = 65'536 bytes use binary scaling factors.

Actually it seems that storage device manufacturers have a rather opportunistic way of switching between decimal and binary scaling factors as a footnote from the *Western Digital Red drive* datasheet shows:
>As used for storage capacity, one megabyte (MB) = one million bytes, one gigabyte (GB) = one billion bytes, and one terabyte (TB) = one trillion bytes. As used for buffer or cache, one megabyte (MB) = 1'048'576 bytes. As used for transfer rate or interface, megabyte per second (MB/s) = one million bytes per second, and gigabit per second (Gb/s).

It is assumed that the [Utah Data Center][UDC] run by the National Security Agency (NSA) has a storage capacity of about 3 to 12 exabytes. And [IDC][IDC] predicts that the collective sum of the world’s data will grow from 33 zettabytes in 2018 to 175 ZB by 2025.

[UDC]: https://en.wikipedia.org/wiki/Utah_Data_Center
[IDC]: https://www.seagate.com/files/www-content/our-story/trends/files/idc-seagate-dataage-whitepaper.pdf

## Bits, Bytes and Words <a name="section3"></a>

The  ASCII graphics below show a 32 bit data word in so called *network order* as defined by [RFC 1700][RFC_1700]. Network order is used to transmit data over communication links in a platform-independent way.

``` <!-- language: lang-none -->
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1  32 bits
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |  8 hex nibbles
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |    byte 0     |    byte 1     |    byte 2     |    byte 3     |  4 bytes
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                         32 bit word                           |  1 word
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```
The left-most `byte 0` which is the most significant byte (MSB) in the `32 bit word` is transmitted first, followed by `byte 1`, then `byte 2` and at last `byte 3`, the least significant byte (LSB). Within the left-most `byte 0`,  the left-most `bit 0` is the most significant bit that is transmitted first and `bit 7`,  the least significant bit, is transmitted last.

Each byte can be represented by 2 hexadecimal nibbles so that a 32 bit word comprising 4 bytes totals 8 nibbles. 

[RFC_1700]: https://tools.ietf.org/html/rfc1700

## Base Conversions <a name="section4"></a>

### Conversion from Decimal Representation

**Python 1**: <a name="python1"></a>Base conversions from decimal to binary, octal or hexadecimal format are a piece of cake

```python
>>> bin(703)
'0b1010111111'
>>> oct(703)
'0o1277'
>>> hex(703)
'0x2bf'
```
The following expression formats a binary number with a constant length of 16 bits
```python
>>> format(703, '#018b')
'0b0000001010111111'
```
In a similar fashion we can format a hexadecimal number with  a constant length of 4 nibbles
```python
>>> format(703, '#06x')
'0x02bf'
```
And if we want to get rid of the base prefixes
```python
>>> format(703, '016b')
'0000001010111111'
>>> format(703, '04x')
'02bf'
```
The following expression stores the bits of a 16 bit integer in an array
```python
>>> [int(x) for x in format(703, '016b')]
[0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1]
```
that can be used for further processing
```python
>>> bits = [int(x) for x in format(703, '016b')]
>>> print('msb:', bits[0], 'lsb:', bits[15])
msb: 0 lsb: 1
```
We can also directly pick individual bits using the shift-right operator `>>` and the *single bit mask* `0x0001`
```python
>>> value = 703
>>> mask = 0x0001
>>> value & mask           # lsb
1
>>> (value >> 6) & mask    # bit at b*2^6
0
>>> (value >> 7) & mask    # bit at b*2^7
1
>>> (value >> 8) & mask    # bit at b*2^8
0
>>> (value >> 9) & mask    # bit at b*2^9
1
>>> (value >> 15) & mask   # msb
0
```
Since a hex number contains the characters `A` to `F` we have to analyze the nibbles as a string
```python
>>> nibbles = format(703, '04X')
>>> print(nibbles[0], nibbles[1], nibbles[2], nibbles[3])
0 2 B F
```
or we can convert each nibble into an integer
```python
>>> [int(x,16) for x in format(703, '04x')]
[0, 2, 11, 15]
```
Using the *byte mask* `0x00ff` we can determine the decimal value of each byte
```python
>>> value = 703
>>> mask = 0x00ff
>>> value & mask          # lower byte
191
>>> (value >> 8) & mask   # upper byte
2
```

### Conversion to Decimal Representation

**Python 2**: <a name="python2"></a>In the reverse direction, binary, octal or hexadecimal *constants* can be displayed as decimal values

```python
>>> print(0b1010111111)
703
>>> print(0o1277)
703
>>> print(0x2bf)
703
```
Usually numbers in various formats are input as *character strings* which can be converted to an integer using the `int(value, base)` function
```python
>>> int('0b1010111111', 2)
703
>>> int('0o1277', 8)
703
>>> int('0x2bf', 16)
703
>>> int('703', 10)
703
>>> int('703')
703
```
Since the base is given, the *base prefix is* not required
```python
>>> int('1010111111', 2)
703
>>> int('1277', 8)
703
>>> int('2bf', 16)
703
```
Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/

