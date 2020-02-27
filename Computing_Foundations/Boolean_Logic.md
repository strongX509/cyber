# Boolean Logic

## Table of Contents
1. [Booleans](#section1)
2. [The AND Operator](#section2)
3. [The OR Operator](#section3)
4. [The XOR Operator](#section4)
5. [Binary Full-Adder](#section5)
6. [The NOT Operator](#section6)
7. [The NAND Operator](#section7)
8. [The NOR Operator](#section8)
9. [De Morgan's Theorems](#section9)
10. [Python Logical Operator Precedence](#section10)
11. [Logic Gate Symbols](#section11)

Python exercises: &nbsp; [PY1](#python1) &nbsp; [PY2](#python2) &nbsp; [PY3](#python3) &nbsp; [PY4](#python4) &nbsp; [PY5](#python5) &nbsp; [PY6](#python6) &nbsp; [PY7](#python7) &nbsp; [PY8](#python8) &nbsp; [PY9](#python9)

## Booleans <a name="section1"></a>

Boolean values, logic and algebra are named after the British mathematician [George Boole][BOOL] (1815-1864).

Booleans can take on only two binary values:

* `0` and `1`
* `False` and `True`
* `FALSE` and `TRUE`
* `No` and `Yes`
* `Off` and `On`

Boolean logic can easily be implemented by electronic circuits using transistors and thus forms the basis for all electronic computers.

**Python 1**: <a name="python1"></a>Booleans are represented either by the integers `0` and `1`:
```python
>>> x = 0
>>> y = 1
>>> print('x:', x, 'y:', y, 'and:', x and y, 'or:', x or y, 'xor:', x ^ y)
x: 0 y: 1 and: 0 or: 1 xor: 1
```
or by the keywords `False` and `True`:
``` python
>>> x = False
>>> y = True
>>> print('x:', x, 'y:', y, 'and:', x and y, 'or:', x or y, 'xor:', x != y, 'not:', not x)
x: False y: True and: False or: True xor: True not: True
```
[BOOL]: https://en.wikipedia.org/wiki/George_Boole

## The AND Operator <a name="section2"></a>

The Boolean AND operator `z = x AND y` has the following truth table:

| x | y |  z  |
|---|---|-----|
| 0 | 0 |**0**|
| 0 | 1 |**0**|
| 1 | 0 |**0**|
| 1 | 1 |**1**|

**Rule**:  The output `z` is  1 (TRUE) only if both inputs `x` and `y` are 1 (TRUE).

The AND operation can also be interpreted as the binary multiplication `z = x * y`.

**Python 2**: <a name="python2"></a>Using the `&` operator, the AND operation can be applied to two bit arrays `x = 0b0011` and `y = 0b0101` both having a size of 4 bits to compute the output array `z = 0b0001` in the following *bit for bit* or *bitwise*  manner:

```
x:  0   0   1   1
   AND AND AND AND
y:  0   1   0   1
   ---------------
z:  0   0   0   1
```
The single python command below computes this *bitwise* AND of the two input bit arrays
```python
>>> x = 0b0011
>>> y = 0b0101
>>> format(x & y, '#06b')
'0b0001'
```
and actually generates all four possible states of the AND truth table listed above.

The auxiliary format parameter`#06b` controls the output of a minimum of `6` characters, starting with the `0b` base-2 prefix (activated by `#`) and followed by 4 binary bits with leading zeroes (activated by `0`).

The following example shows how the `AND` operator can be used to test the flags at positions `0` and `5` in a bit array by applying a corresponding bit mask:
```
        7   6   5   4   3   2   1   0
      +---+---+---+---+---+---+---+---+
flags | 0 | 0 | 1 | 0 | 0 | 1 | 1 | 0 |
      +---+---+---+---+---+---+---+---+
mask0 |   |   |   |   |   |   |   | 1 |
      +---+---+---+---+---+---+---+---+
mask5 |   |   | 1 |   |   |   |   |   |
      +---+---+---+---+---+---+---+---+
```
```python
>>> flags = 0b00100110
>>> (flags & 1) != 0          # test flag at position 0
False
>>> (flags & (1 << 5)) != 0   # test flag at position 5
True
```

## The OR Operator <a name="section3"></a>

The Boolean OR operator `z = x OR y` has the following truth table:

| x | y |  z  |
|---|---|-----|
| 0 | 0 |**0**|
| 0 | 1 |**1**|
| 1 | 0 |**1**|
| 1 | 1 |**1**|

**Rule**: The output `z` is  1 (TRUE) if at least one of the inputs `x` and `y` are 1 (TRUE).

**Python 3:** <a name="python3"></a>Using the `|` operator, the OR operation can be applied in a *bitwise* manner to the two bit arrays represented by the integers `0b0011` and `0b0101`
```python
>>> x = 0b0011
>>> y = 0b0101
>>> format(x | y, '#06b')
'0b0111'
```
The following example shows how the `OR` operator can be used to set the flags at positions `0`,  `2` and `7` in a bit array by applying a corresponding bit mask:
```
        7   6   5   4   3   2   1   0
      +---+---+---+---+---+---+---+---+
flags | 0 | 0 | 1 | 0 | 0 | 1 | 1 | 0 |
      +---+---+---+---+---+---+---+---+
mask  | 1  |  |   |   |   | 1 |   | 1 |
      +---+---+---+---+---+---+---+---+
```
```python
>>> flags = 0b00100110
>>> format(flags, '#010b')
'0b00100110'
>>> mask = 1 | (1 << 2) | (1 << 7)
>>> format(mask, '#010b')
'0b10000101'
>>> flags |= mask
>>> format(flags, '#010b')
'0b10100111'
```

## The XOR Operator <a name="section4"></a>

The Boolean XOR operator `z = x XOR y` has the following truth table:

| x | y |  z  |
|---|---|-----|
| 0 | 0 |**0**|
| 0 | 1 |**1**|
| 1 | 0 |**1**|
| 1 | 1 |**0**|

**Rule**: The output `z` is  1 (TRUE) if the values of the inputs `x` and `y` are differing.

The XOR operation can thus be interpreted as the inequality operation `z = x != y`.

Therefore `x XOR x = 0` and it follows that `x XOR y XOR x = y XOR x XOR x = y XOR 0 = y`, i.e. `y` can always be retrieved after XOR-ing it with `x`, by XOR-ing the result with `x` again. This property is used e.g. by *stream ciphers* in crypto devices and *data scramblers* in communication systems.

**Python 4:** <a name="python4"></a>Using the `^` operator, the XOR operation can be applied in a *bitwise* manner to the two bit arrays represented by the integers `0b0011` and `0b0101`

```python
>>> x = 0b0011
>>> y = 0b0101
>>> format(x ^ y, '#06b')
'0b0110'
```
A bit array XOR-ed with itself always results in an all-zeroes array
```python
>>> x = 0b0011
>>> format(x ^ x, '#06b')
'0b0000'
```
The *associativity* and *commutativity*  properties of the XOR operator are used  in the following *stream cipher* example:
```python
>>> x = 0b0011
>>> y = 0b0101
>>> format(x ^ y ^ x, '#06b')
'0b0101'
```
The plaintext bits `0b0101` are encrypted on the transmitting side by the secret cipher stream bits `0b0011` and decrypted on the receiving side by applying the same cipher stream sequence `0b0011` again.

## Binary Full-Adder <a name="section5"></a>

The binary [full-adder][ADDER] digital circuit shown below adds the binary bits `A` and `B` plus the carry bit `Cin` from the previous adder stage. The output is the sum `S` and the carry bit `Cout` to be considered by the next adder stage.

!["Binary Full Adder"][ADDER_IMG]

The sum is computed as `S = Z XOR Cin`, using the intermediate sum `Z = A XOR B`. Apparently the XOR operator is equivalent to the binary addition `z = (x + y) mod 2`. The output carry is computed as `Cout = (Z AND Cin) OR (A AND B)` and uses the intermediate sum as well.

The full-adder has the following truth table:

| A | B |Cin|Cout |  S  |
|---|---|---|-----|-----|
| 0 | 0 | 0 |**0**|**0**|
| 0 | 0 | 1 |**0**|**1**|
| 0 | 1 | 0 |**0**|**1**|
| 0 | 1 | 1 |**1**|**0**|
| 1 | 0 | 0 |**0**|**1**|
| 1 | 0 | 1 |**1**|**0**|
| 1 | 1 | 0 |**1**|**0**|
| 1 | 1 | 1 |**1**|**1**|

**Python 5:** <a name="python5"></a>We test all states of the full-adder's truth table:

```python
>>> a = 0b00001111
>>> b = 0b00110011
>>> z = a ^ b
>>> cin = 0b01010101
>>> s = z ^ cin
>>> cout = (z & cin) | (a & b)
>>> format(a, '08b')
'00001111'
>>> format(b, '08b')
'00110011'
>>> format(z, '08b')
'00111100'
>>> format(cin, '08b')
'01010101'
>>> format(cout, '08b')
'00010111'
>>> format(s, '08b')
'01101001'
```
[ADDER]: https://en.wikipedia.org/wiki/Adder_(electronics)
[ADDER_IMG]: https://upload.wikimedia.org/wikipedia/commons/6/69/Full-adder_logic_diagram.svg "Binary Full Adder, CC0"

## The NOT Operator <a name="section6"></a>

The Boolean NOT operator `z = NOT x` has the following truth table:

| x |  z  |
|---|-----|
| 0 |**1**|
| 1 |**0**|

**Rule**: The output `z` is the inverse or complement of the input `x`.

**Python 6:** <a name="python6"></a>Using the `~` operator, the NOT operation can be applied in a *bitwise* manner to the bit array represented by the integer `0b0011`

```python
>>> x = 0b0011
>>> format(~x & 0b1111, '#06b')
'0b1100'
```
Since Python treats the bit array as a signed integer, a bit mask `0b1111` corresponding to the number of elements in the bit array must be applied after the inversion in order to convert the result into an unsigned integer.

## The NAND Operator <a name="section7"></a>

The Boolean NAND operator `z = x NAND y = NOT (x AND y)` has the following truth table:

| x | y |  z  |
|---|---|-----|
| 0 | 0 |**1**|
| 0 | 1 |**1**|
| 1 | 0 |**1**|
| 1 | 1 |**0**|

**Rule**:  The output `z` is  0 (FALSE) only if both inputs `x` and `y` are 1 (TRUE).

The NAND operation is equivalent to an inverted AND operation and is
implemented by transitor-based NAND gates used in electronic circuitry.

**Python 7:** <a name="python7"></a>Using the `~` and `&` operators, the NAND operation can be applied in a *bitwise* manner to the two bit arrays represented by the integers `0b0011` and `0b0101`

```python
>>> x = 0b0011
>>> y = 0b0101
>>> format(~(x & y) & 0b1111, '#06b')
'0b1110'
```
Since Python treats the bit array as a signed integer, a bit mask `0b1111` corresponding to the number of elements in the bit array must be applied after the inversion in order to convert the result into an unsigned integer.

## The NOR Operator <a name="section8"></a>

The Boolean NOR operator `z = x NOR y = NOT (x OR y)` has the following truth table:

| x | y |  z  |
|---|---|-----|
| 0 | 0 |**1**|
| 0 | 1 |**0**|
| 1 | 0 |**0**|
| 1 | 1 |**0**|

**Rule**: The output `z` is  0 (FALSE) if at least one of the inputs `x` and `y` are 1 (TRUE).

The NOR operation is equivalent to an inverted OR operation and is implemented by transitor-based NOR gates used in electronic circuitry.

**Python 8:**<a name="python8"></a> Using the `~` and `|` operators, the NOR operation can be applied in a *bitwise* manner to the two bit arrays represented by the integers `0b0011` and `0b0101`

```python
>>> x = 0b0011
>>> y = 0b0101
>>> format(~(x | y) & 0b1111, '#06b')
'0b1000'
```
Since Python treats the bit array as a signed integer, a bit mask `0b1111` corresponding to the number of elements in the bit array must be applied after the inversion in order to convert the result into an unsigned integer.

## De Morgan's Theorems <a name="section9"></a>

Two theorems formulated by [Augustus de Morgan][MORG] (1806-1871) sometimes allow the simplification of complex Boolean expressions:

* The first DeMorgan theorem states the relationship `NOT (x AND y) = (NOT x) OR (NOT y)`, i.e. the NAND operation is equivalent to the OR operation with inverted inputs.

* The second DeMorgan theorem states the dual relationship `NOT (x OR y) = (NOT x) AND (NOT y)`, i.e. the NOR operation is equivalent to the AND operation with inverted inputs.

**Python 9**: <a name="python9"></a>We implement these two relationships using the *bitwise* operators `&`, `|` and `~` and verify the equality of the left and right side of the expressions using the *bitwise* inequality operator `^`:

```python
>>> x = 0b0011
>>> y = 0b0101
>>> format(~(x & y) ^ (~x | ~y), '#06b')
'0b0000'
>>> format(~(x | y) ^ (~x & ~y), '#06b')
'0b0000'
```
[MORG]: https://en.wikipedia.org/wiki/Augustus_De_Morgan

## Python Logical Operator Precedence <a name="section10"></a>

The precedence (priority) of the Python logical operators is as follows:

1. `()` - Parentheses
2. `~`  - Bitwise NOT
3. `<<`,  `>>` - Bitwise shifts
4. `&`  - Bitwise AND
5. `^`  - Bitwise XOR
6. `|`  - Bitwise OR
7. `==`, `!=` - Comparisons
8. `not` - Boolean NOT
9. `and` - Boolean AND
10. `or`  - Boolean OR

If in doubt add additional parentheses!

## Logic Gate Symbols <a name="section11"></a>

| Logic Function |       Gate Symbol         |
| :------------: | :-----------------------: |
|      NOT       | !["NOT gate"][NOT_GATE]   |
|      AND       | !["AND gate"][AND_GATE]   |
|      OR        | !["OR gate"][OR_GATE]     |
|      XOR       | !["XOR gate"][XOR_GATE]   |
|      NAND      | !["NAND gate"][NAND_GATE] |
|      NOR       | !["NOR gate"][NOR_GATE]   |

[NOT_GATE]: https://upload.wikimedia.org/wikipedia/commons/6/60/NOT_ANSI_Labelled.svg "CC0"
[AND_GATE]: https://upload.wikimedia.org/wikipedia/commons/b/b9/AND_ANSI_Labelled.svg "CC0"
[OR_GATE]: https://upload.wikimedia.org/wikipedia/commons/1/16/OR_ANSI_Labelled.svg "CC0"
[XOR_GATE]: https://upload.wikimedia.org/wikipedia/commons/1/17/XOR_ANSI_Labelled.svg "CC0"
[NAND_GATE]: https://upload.wikimedia.org/wikipedia/commons/e/e6/NAND_ANSI_Labelled.svg  "CC0"
[NOR_GATE]: https://upload.wikimedia.org/wikipedia/commons/c/c6/NOR_ANSI_Labelled.svg "CC0"

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/