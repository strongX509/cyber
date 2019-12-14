## Representation of Natural Numbers

The [natural numbers](https://en.wikipedia.org/wiki/Natural_number) consist of all positive integers 1, 2, 3, ... Often zero is also included, so that the natural numbers cover all non-negative integers 0, 1, 2, 3, ...

Natural numbers are usually represented in the [decimal numeral system](https://en.wikipedia.org/wiki/Decimal) or base-10 [positional numeral system](https://en.wikipedia.org/wiki/Positional_notation):
```
703 = 7*10^2 + 0*10^1 + 3*10^0
```
But besides the human possessing 10 fingers on his two hands there is no special advantage in using 10 as a base for a [positional numeral system](https://en.wikipedia.org/wiki/Positional_notation). The generic positional notation for an arbitrary number base is shown below.

![Number = Digits_Radix](https://upload.wikimedia.org/wikipedia/commons/7/78/Positional_notation_glossary-en.svg "Terms used in positional numeral systems")

Actually for a computer built from electronic NAND and NOR gates implementing Boolean logic, the base-2 [binary numeral system](https://en.wikipedia.org/wiki/Binary_number) is a much more convenient number representation:
```
1010111111 = 1*2^9 + 0*2^8 + 1*2^7 + 0*2^6 + 1*2^5 + 1*2^4 + 1*2^3 + 1*2^2 + 1*2^1 + 1*2^0
```
For humans the many digits of a binary number representation are quite tiresome. Therefore the more compact base-8 [octal numeral system](https://en.wikipedia.org/wiki/Octal)
```
1277 = 1*8^3 + 2*8^2 + 7*8^1 + 7*8^0
```
or the very popular base-16 [hexadecimal positional numeral system](https://en.wikipedia.org/wiki/Hexadecimal)
```
2BF = 2*16^2 + B*16^1 + F*16^0
```
are extensively used in computer and software engineering.

## Positional Numeral Systems

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

|   n  |    10^n |Units|   k  |      2^k |  i   |      8^i |   m  |     16^m |
|-----:|--------:|-----|-----:|---------:|-----:|---------:|-----:|---------:|
| **0**|        1|     | **0**|        1 | **0**|         1| **0**|        1 |
|      |         |     | **1**|        2 |   -  |          |   -  |          |
|      |         |     | **2**|        4 |   -  |          |   -  |          |
|      |         |     | **3**|        8 | **1**|         8|   -  |          |
| **1**|       10|     |   -  |          |   -  |          |   -  |          |
|      |         |     | **4**|       16 |   -  |          | **1**|       16 |
|      |         |     | **5**|       32 |   -  |          |   -  |          |
|      |         |     | **6**|       64 | **2**|        64|   -  |          |
| **2**|      100|     |   -  |          |   -  |          |   -  |          |
|      |         |     | **7**|      128 |   -  |          |   -  |          |
|      |         |     | **8**|      256 |   -  |          | **2**|      256 |
|      |         |     | **9**|      512 | **3**|       512|   -  |          |
| **3**|     1000|kilo |**10**|    1'024 |   -  |          |   -  |          |
|      |         |     |**11**|    2'048 |   -  |          |   -  |          |
|      |         |     |**12**|    4'096 | **4**|     4'096| **3**|    4'096 |
|      |         |     |**13**|    8'192 |   -  |          |   -  |          |
|      |         |     |**14**|   16'384 |   -  |          |   -  |          |
|      |         |     |**15**|   32'768 | **5**|    32'768|   -  |          |
|      |         |     |**16**|   65'536 |   -  |          | **4**|   65'536 |
| **6**|1'000'000|Mega |**20**|1'048'576 |   -  |          | **5**|1'048'576 |
|      |         |     |**24**|&#8776;16M| **8**|&#8776;16M| **6**|&#8776;16M|
| **9**|       1G|Giga |**30**|&#8776;1G |**10**|&#8776;1G |   -  |          |
|      |         |     |**32**|&#8776;4G |   -  |          | **8**|&#8776;4G |
|**12**|       1T|Tera |**40**|&#8776;1T |   -  |          |**10**|&#8776;1T |
|**15**|       1P|Peta |**50**|&#8776;1P |   -  |          |   -  |          |
|**18**|       1E|Exa  |**60**|&#8776;1E |**20**|&#8776;1E |**15**|&#8776;1E |
|      |         |     |**64**|&#8776;16E|   -  |          |**16**|&#8776;16E|
|**21**|       1Z|Zeta |**70**|&#8776;1Z |   -  |          |   -  |          |
|**24**|       1Y|Yotta|**80**|&#8776;1Y |   -  |          |**20**|&#8776;1Y |

## Base Conversions

