# Character Sets

## Table of Contents
1. [ASCII](#section1)
2. [UTF-8](#section2)

Python exercises: &nbsp; [PY1](#python1) &nbsp; [PY2](#python2)

## ASCII <a name="section1"></a>

Still one of the widely used character encodings for electronic communication is [ASCII][ASCII] , the *American Standard Code for Information Interchange*. The output of `man ascii` shown below lists the original 7-bit US-ASCII code covering only latin characters used in English plus a lot of control characters used in the early days of modem links, teleprinters and console terminals.

|Oct|Dec|Hex|Char                     |Oct|Dec|Hex|Char                   |
|:-:|--:|:-:|:------------------------|:-:|--:|:-:|:----------------------|
|000|  0| 00|NUL '\0' (null character)|020| 16| 10|DLE (data link escape) |
|001|  1| 01|SOH (start of heading)   |021| 17| 11|DC1 (device control 1) |
|002|  2| 02|STX (start of text)      |022| 18| 12|DC2 (device control 2) |
|003|  3| 03|ETX (end of text)        |023| 19| 13|DC3 (device control 3) |
|004|  4| 04|EOT (end of transmission)|024| 20| 14|DC4 (device control 4) |
|005|  5| 05|ENQ (enquiry)            |025| 21| 15|NAK (negative ack.)    |
|006|  6| 06|ACK (acknowledge)        |026| 22| 16|SYN (synchronous idle) |
|007|  7| 07|BEL '\a' (bell)          |027| 23| 17|ETB (end of trans. blk)|
|010|  8| 08|BS  '\b' (backspace)     |030| 24| 18|CAN (cancel)           |
|011|  9| 09|HT  '\t' (horizontal tab)|031| 25| 19|EM  (end of medium)    |
|012| 10| 0A|LF  '\n' (new line)      |032| 26| 1A|SUB (substitute)       |
|013| 11| 0B|VT  '\v' (vertical tab)  |033| 27| 1B|ESC (escape)           |
|014| 12| 0C|FF  '\f' (form feed)     |034| 28| 1C|FS  (file separator)   |
|015| 13| 0D|CR  '\r' (carriage ret)  |035| 29| 1D|GS  (group separator)  |
|016| 14| 0E|SO  (shift out)          |036| 30| 1E|RS  (record separator) |
|017| 15| 0F|SI  (shift in)           |037| 31| 1F|US  (unit separator)   |

SPACE and numbers 0 to 9:

|Oct|Dec|Hex|Char    |Oct|Dec|Hex|Char    |
|:-:|--:|:-:|:-------|:-:|--:|:-:|:-------|
|040| 32| 20|SPACE   |060| 48| 30| 0      |
|041| 33| 21| !      |061| 49| 31| 1      |
|042| 34| 22| "      |062| 50| 32| 2      |
|043| 35| 23| #      |063| 51| 33| 3      |
|044| 36| 24| $      |064| 52| 34| 4      |
|045| 37| 25| %      |065| 53| 35| 5      |
|046| 38| 26| &      |066| 54| 36| 6      |
|047| 39| 27| '      |067| 55| 37| 7      |
|050| 40| 28| (      |070| 56| 38| 8      |
|051| 41| 29| )      |071| 57| 39| 9      |
|052| 42| 2A| *      |072| 58| 3A| :      |
|053| 43| 2B| +      |073| 59| 3B| ;      |
|054| 44| 2C| ,      |074| 60| 3C| <      |
|055| 45| 2D| -      |075| 61| 3D| =      |
|056| 46| 2E| .      |076| 62| 3E| >      |
|057| 47| 2F| /      |077| 63| 3F| ?      |

Capital letters A to Z and small letters a to z:

|Oct|Dec|Hex|Char    |Oct|Dec|Hex|Char    |
|:-:|--:|:-:|:-------|:-:|--:|:-:|:-------|
|100| 64| 40| @      |140| 96| 60| `      |
|101| 65| 41| A      |141| 97| 61| a      |
|102| 66| 42| B      |142| 98| 62| b      |
|103| 67| 43| C      |143| 99| 63| c      |
|104| 68| 44| D      |144|100| 64| d      |
|105| 69| 45| E      |145|101| 65| e      |
|106| 70| 46| F      |146|102| 66| f      |
|107| 71| 47| G      |147|103| 67| g      |
|110| 72| 48| H      |150|104| 68| h      |
|111| 73| 49| I      |151|105| 69| i      |
|112| 74| 4A| J      |152|106| 6A| j      |
|113| 75| 4B| K      |153|107| 6B| k      |
|114| 76| 4C| L      |154|108| 6C| l      |
|115| 77| 4D| M      |155|109| 6D| m      |
|116| 78| 4E| N      |156|110| 6E| n      |
|117| 79| 4F| O      |157|111| 6F| o      |
|120| 80| 50| P      |160|112| 70| p      |
|121| 81| 51| Q      |161|113| 71| q      |
|122| 82| 52| R      |162|114| 72| r      |
|123| 83| 53| S      |163|115| 73| s      |
|124| 84| 54| T      |164|116| 74| t      |
|125| 85| 55| U      |165|117| 75| u      |
|126| 86| 56| V      |166|118| 76| v      |
|127| 87| 57| W      |167|119| 77| w      |
|130| 88| 58| X      |170|120| 78| x      |
|131| 89| 59| Y      |171|121| 79| y      |
|132| 90| 5A| Z      |172|122| 7A| z      |
|133| 91| 5B| [      |173|123| 7B| {      |
|134| 92| 5C| \  '\\'|174|124| 7C| \|     |
|135| 93| 5D| ]      |175|125| 7D| }      |
|136| 94| 5E| ^      |176|126| 7E| ~      |
|137| 95| 5F| _      |177|127| 7F|DEL     |

The extended ASCII code uses 8 bits thus increasing the code table to `2^8 = 256` characters in order to cover characters in other languages, e.g. the ISO 8859 Latin-1 extension that covers latin characters in other western languages: à ä e e ê ô ö ù ü …

**Python 1**: <a name="python1"></a>

We generate a  byte array from an ASCII string containing the numbers `0` to `9`
```python
b_num = bytearray("0123456789", "ascii")
>>> print(b_num, len(b_num))
bytearray(b'0123456789') 10
>>> [c for c in b_num]
[48, 49, 50, 51, 52, 53, 54, 55, 56, 57]                      # dec
>>> b_num[9]
57
>>> [format(c, '02x') for c in b_num]
['30', '31', '32', '33', '34', '35', '36', '37', '38', '39']  # hex
```

We generate a  byte array from an ASCII string containing the uppercase characters `A` to `Z`
```python
>>> b_chr = bytearray("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "ascii")
>>> print(b_chr, len(b_chr))
bytearray(b'ABCDEFGHIJKLMNOPQRSTUVWXYZ') 26
>>> [c for c in b_chr]
[65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90]
>>> b_chr[0]
65
>>> [format(c, '02x') for c in b_chr]
['41', '42', '43', '44', '45', '46', '47', '48', '49', '4a', '4b', '4c', '4d', '4e', '4f', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '5a']
```
We load an ASCII string into a byte array, convert the byte array into a hex string and convert the hex string back into an ASCII string.
```python
>>> import binascii
>>> num_str = '0123456789'
>>> num_arr = bytearray(num_str, 'ascii')
>>> print(num_arr)
bytearray(b'0123456789')
>>> num_hex = binascii.hexlify(num_arr)
>>> print(num_hex)
b'30313233343536373839'
>>> binascii.unhexlify(hex_num)
b'0123456789'
```

The following python script `ascii.py` capitalizes lowercase characters in a byte array by unsetting `bit 5` in the ASCII encoding and then converts all uppercase characters to lowercase by setting `bit 5`.
```python
#!/usr/bin/python3
b_arr = bytearray("Oh my god! There are 20 students in the classroom, (\x07)", "ascii")
print(b_arr)

# Capitalize lower case characters by unsetting bit 5
for i in range(0, len(b_arr)):
    if b_arr[i] > 0x60 and b_arr[i] < 0x7b:
        b_arr[i] &= 0xdf
print(b_arr)

# Convert all uppercase characters to lowercase by setting bit 5
for i in range(0, len(b_arr)):
    if b_arr[i] > 0x40 and b_arr[i] < 0x5b:
        b_arr[i] |= 0x20
print(b_arr)
```
The output of the script is
```python
./ascii.py
bytearray(b'Oh my god! There are 20 students in the classroom, (\x07)')
bytearray(b'OH MY GOD! THERE ARE 20 STUDENTS IN THE CLASSROOM, (\x07)')
bytearray(b'oh my god! there are 20 students in the classroom, (\x07)')
```

[ASCII]: https://en.wikipedia.org/wiki/ASCII

## UTF-8 <a name="section2"></a>

**Python 2**: <a name="python2"></a>

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/

