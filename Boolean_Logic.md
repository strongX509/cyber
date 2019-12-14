--------------------------------------------------------------------------------------------------------
## Booleans

Boolean values, logic and algebra are named after the British mathematician [George Boole](https://en.wikipedia.org/wiki/George_Boole) (1815-1864).

Booleans can take on only two binary values:

* `0` and `1`
* `False` and `True`
* `FALSE` and `TRUE`
* `No` and `Yes`
* `Off` and `On`

Boolean logic can easily be implemented by electronic circuits using transistors and thus forms the basis for all electronic computers.

**Python**: Booleans are represented either by the integers `0` and `1`:
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
--------------------------------------------------------------------------------------------------------
## The AND Operator

The Boolean AND operator `z = x AND y` has the following truth table:

| x | y |  z  |
|---|---|-----|
| 0 | 0 |**0**|
| 0 | 1 |**0**|
| 1 | 0 |**0**|
| 1 | 1 |**1**|

The AND operation can also be interpreted as the binary multiplication `z = x * y`.

**Python:** Using the `&` operator, the AND operation can be applied to a bit array stored in a data word in a *bitwise* manner 
```python
>>> format(0b0011 & 0b0101, '#06b')
'0b0001'
```
--------------------------------------------------------------------------------------------------------
## The OR Operator

The Boolean OR operator `z = x OR y` has the following truth table:

| x | y |  z  |
|---|---|-----|
| 0 | 0 |**0**|
| 0 | 1 |**1**|
| 1 | 0 |**1**|
| 1 | 1 |**1**|

**Python:** Using the `|` operator, the OR operation can be applied to a bit array stored in a data word in a *bitwise* manner 
```python
>>> format(0b0011 | 0b0101, '#06b')
'0b0111'
```
--------------------------------------------------------------------------------------------------------
## The XOR Operator

The Boolean AND operator `z = x XOR y` has the following truth table:

| x | y |  z  |
|---|---|-----|
| 0 | 0 |**0**|
| 0 | 1 |**1**|
| 1 | 0 |**1**|
| 1 | 1 |**0**|

The XOR operation can also be interpreted as the inequality operation `z = x != y`.

Therefore `x XOR x = 0` and it follows that `x XOR y XOR x = y XOR x XOR x = y XOR 0 = y`, i.e. `y` can always be retrieved after XOR-ing it with `x`, by XOR-ing the result with `x` again. This property is used e.g. by *stream ciphers* in crypto devices and *data scramblers* in communication systems.

**Python:** Using the `^` operator, the XOR operation can be applied to a bit array stored in a data word in a *bitwise* manner 
```python
>>> format(0b0011 ^ 0b0101, '#06b')
'0b0110'
```
The *associativity*, *commutativity* and *inequality* properties of the XOR operator are shown in the following *stream cipher* example:
```python
>>> format(0b0011 ^ 0b0101 ^ 0b0011, '#06b')
'0b0101'
```
The plaintext bits `0b0101` are encrypted on the transmitting side by the secret cipher stream bits `0b0011` and decrypted on the receiving side by applying the same cipher stream sequence `0b0011` again.
--------------------------------------------------------------------------------------------------------
## The NOT Operator

The Boolean NOT operator `z = NOT x` has the following truth table:

| x |  z  |
|---|-----|
| 0 |**1**|
| 1 |**0**|

The NOT operation inverts the Boolean input value.

**Python:** Using the `~` operator, the NOT operation can be applied to a bit array stored in a data word in a bitwise manner 
```python
>>> format(~0b0011 & 0b1111, '#06b')
'0b1100'
```
Since Python treats the bit array as a signed integer, a bit mask `0b1111` corresponding to the number of elements in the bit array must be applied after the inversion in order to convert the result into an unsigned integer.
--------------------------------------------------------------------------------------------------------
## The NAND Operator

The Boolean NAND operator `z = x NAND y = NOT (x AND y)` has the following truth table:

| x | y |  z  |
|---|---|-----|
| 0 | 0 |**1**|
| 0 | 1 |**1**|
| 1 | 0 |**1**|
| 1 | 1 |**0**|

The NAND operation is equivalent to an inverted AND operation and is
implemented by transitor-based NAND gates used in electronic circuitry.

**Python:** Using the `~` and `&` operators, the NAND operation can be applied to a bit array stored in a data word in a bitwise manner 
```python
>>> format(~(0b0011 & 0b0101) & 0b1111, '#06b')
'0b1110'
```
Since Python treats the bit array as a signed integer, a bit mask `0b1111` corresponding to the number of elements in the bit array must be applied after the inversion in order to convert the result into an unsigned integer.
--------------------------------------------------------------------------------------------------------
## The NOR Operator

The Boolean NOR operator `z = x NOR y = NOT (x OR y)` has the following truth table:

| x | y |  z  |
|---|---|-----|
| 0 | 0 |**1**|
| 0 | 1 |**0**|
| 1 | 0 |**0**|
| 1 | 1 |**0**|

The NOR operation is equivalent to an inverted OR operation and is
implemented by transitor-based NOR gates used in electronic circuitry.

**Python:** Using the `~` and `|` operators, the NAND operation can be applied to a bit array stored in a data word in a bitwise manner 
```python
>>> format(~(0b0011 | 0b0101) & 0b1111, '#06b')
'0b1000'
```
Since Python treats the bit array as a signed integer, a bit mask `0b1111` corresponding to the number of elements in the bit array must be applied after the inversion in order to convert the result into an unsigned integer.
--------------------------------------------------------------------------------------------------------
## DeMorgan's Theorems

Two theorems formulated by [Augustus de Morgan](https://en.wikipedia.org/wiki/Augustus_De_Morgan) (1806-1871) sometimes allow the simplification of complex Boolean expressions:

* The first DeMorgan theorem states the relationship `NOT (x AND y) = (NOT x) OR (NOT y)`, i.e. the NAND operation is equivalent to the OR operation with inverted inputs.

* The second DeMorgan theorem states the dual relationship `NOT (x OR y) = (NOT x) AND (NOT y)`, i.e. the NOR operation is equivalent to the AND operation with inverted inputs.

**Python**: We implement these two relationships using the *bitwise* operators `&`, `|` and `~` and verify the equality of the left and right side of the expressions using the *bitwise* inequality operator `^`:
```python
>>> x = 0b0011
>>> y = 0b0101
>>> format(~(x & y) ^ (~x | ~y), '#06b')
'0b0000'
>>> format(~(x | y) ^ (~x & ~y), '#06b')
'0b0000'
```
--------------------------------------------------------------------------------------------------------
## Python Logical Operator Precedence

The precedence (priority) of the Python logical operators is as follows:

1. `()` - Parentheses
2. `~`  - Bitwise NOT
3. `&`  - Bitwise AND
4. `^`  - Bitwise XOR
5. `|`  - Bitwise OR
6. `==`, `!=` - Comparisons
7. `not` - Boolean NOT
8. `and` - Boolean AND
9. `or`  - Boolean OR

If in doubt add additional parentheses!
