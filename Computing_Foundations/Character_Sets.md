# Character Sets

## Table of Contents
1. [ASCII](#section1)
2. [UTF-8](#section2)
3. [Base64](#section3)

Python exercises: &nbsp; [PY1](#python1) &nbsp; [PY2](#python2)&nbsp; [PY3](#python3)

## ASCII <a name="section1"></a>

Still one of the most widely used character encodings for electronic communication is [ASCII][ASCII] , the *American Standard Code for Information Interchange*. The output of `man ascii` shown below lists the original 7-bit US-ASCII code covering only latin characters used in English plus a lot of control characters used in the early days of modem links, teleprinters and console terminals.

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
        b_arr[i] &= 0b11011111
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
As a comparison we show how conversion to uppercase or lowercase can be easily done using Python string functions.
```python
>>> b_str = 'Oh my god! There are 20 students in the classroom, (\x07)'
>>> b_str.upper()
'OH MY GOD! THERE ARE 20 STUDENTS IN THE CLASSROOM, (\x07)'
>>> b_str.lower()
'oh my god! there are 20 students in the classroom, (\x07)'
```

[ASCII]: https://en.wikipedia.org/wiki/ASCII

## UTF-8 <a name="section2"></a>

The 8-bit Unicode Transformation Format [UTF-8][UTF-8] is a variable width character encoding capable of encoding all 1,112,064 valid code points in the *Unicode  (Universal Coded Character Set)* using one to four 8-bit bytes.

It was designed for backward compatibility with ASCII. Code points with lower numerical values, which tend to occur more frequently, are encoded using fewer bytes. The first 128 characters of Unicode, which correspond one-to-one with ASCII, are encoded using a single byte with the same binary value as ASCII, so that valid ASCII text is valid UTF-8-encoded Unicode as well. Since ASCII bytes do not occur when encoding non-ASCII code points into UTF-8, UTF-8 is safe to use within most programming and document languages that interpret certain ASCII characters in a special way, such as "/" (slash) in filenames, "\" (backslash) in escape sequences, and "%" in printf.

Since the restriction of the Unicode code-space to 21-bit values in 2003, UTF-8 is defined to encode code points in one to four bytes, depending on the number of significant bits in the numerical value of the code point. The following table shows the structure of the encoding. The x characters are replaced by the bits of the code point. If the number of significant bits is no more than seven, the first line applies; if no more than 11 bits, the second line applies, and so on.

|Number of Bytes|Bits for code point|First code point|Last code point|Byte 1|Byte 2|Byte 3|Byte 4|
|:-:|:--:|-------:|--------:|--------:|--------:|--------:|--------:|
| 1	|  7 |  U+0000|   U+007F| 0xxxxxxx| 		|         |         |
| 2 | 11 |	U+0080|   U+07FF| 110xxxxx| 10xxxxxx|		  |         |
| 3 | 16 |	U+0800|   U+FFFF| 1110xxxx| 10xxxxxx| 10xxxxxx|         |
| 4	| 21 | U+10000| U+10FFFF| 11110xxx| 10xxxxxx| 10xxxxxx| 10xxxxxx|

* The first 128 characters (US-ASCII) need one byte.
* The next 1,920 characters need two bytes to encode, which covers the remainder of almost all *Latin-script* alphabets, and also *Greek*, *Cyrillic*, *Coptic*, *Armenian*, *Hebrew*, *Arabic*, *Syriac*, *Thaana* and *N'Ko* alphabets, as well as *Combining Diacritical Marks*.
* Three bytes are needed for characters in the rest of the *Basic Multilingual Plane*, which contains virtually all characters in common use, including most *Chinese*, *Japanese* and *Korean* characters.
* Four bytes are needed for characters in the other planes of Unicode, which include less common CJK characters, various historic scripts, *mathematical symbols*, and *emoji* (pictographic symbols).

**Python 2**: <a name="python2"></a>We analyze a some UTF-8 encoding examples.

We start with a very simple example containing the ASCII character `a`, the German character `ä`, the French character `à` and a latin small character a with a ring below `ḁ`:
```python
>>> import binascii
>>> chr_str = u'aäàḁ'
>>> print(chr_str, len(chr_str))
aäàḁ 4
>>> [c for c in chr_str]
['a', 'ä', 'à', 'ḁ']
>>> [binascii.hexlify(bytearray(c, 'utf-8')) for c in chr_str]
[b'61', b'c3a4', b'c3a0', b'e1b881']
>>> chr_arr = bytearray(chr_str, 'utf-8')
>>> print(chr_arr, len(chr_arr))
bytearray(b'a\xc3\xa4\xc3\xa0\xe1\xb8\x81') 8
>>> [format(x, '08b') for x in chr_arr]
['01100001', '11000011', '10100100', '11000011', '10100000', '11100001', '10111000', '10000001']
>>> chr_hex = binascii.hexlify(chr_arr)
>>> print(chr_hex, len(chr_hex))
b'61c3a4c3a0e1b881' 16
```
Here is the UTF-8 encoding of a German [pangram][PAN_DE] containing 35 different characters:
```python
>>> import binascii
>>> chr_str = u'„Fix, Schwyz!“, quäkt Jürgen blöd vom Paß'
>>> print(chr_str, len(chr_str))
„Fix, Schwyz!“, quäkt Jürgen blöd vom Paß 41
>>> [c for c in chr_str]
['„', 'F', 'i', 'x', ',', ' ', 'S', 'c', 'h', 'w', 'y', 'z', '!', '“', ',', ' ', 'q', 'u', 'ä', 'k', 't', ' ', 'J', 'ü', 'r', 'g', 'e', 'n', ' ', 'b', 'l', 'ö', 'd', ' ', 'v', 'o', 'm', ' ', 'P', 'a', 'ß']
[binascii.hexlify(bytearray(c, 'utf-8')) for c in chr_str]
[b'e2809e', b'46', b'69', b'78', b'2c', b'20', b'53', b'63', b'68', b'77', b'79', b'7a', b'21', b'e2809c', b'2c', b'20', b'71', b'75', b'c3a4', b'6b', b'74', b'20', b'4a', b'c3bc', b'72', b'67', b'65', b'6e', b'20', b'62', b'6c', b'c3b6', b'64', b'20', b'76', b'6f', b'6d', b'20', b'50', b'61', b'c39f']
>>> chr_arr = bytearray(chr_str, 'utf-8')
>>> print(chr_arr, len(chr_arr))
bytearray(b'\xe2\x80\x9eFix, Schwyz!\xe2\x80\x9c, qu\xc3\xa4kt J\xc3\xbcrgen bl\xc3\xb6d vom Pa\xc3\x9f') 49
chr_hex = binascii.hexlify(chr_arr)
>>> print(chr_hex, len(chr_hex))
b'e2809e4669782c2053636877797a21e2809c2c207175c3a46b74204ac3bc7267656e20626cc3b66420766f6d205061c39f' 98
```
And here the UTF-8 encoding of a French [pangram][PAN_FR] containing all 45 characters of the French language:
```python
>> import binascii
>>> chr_str = u'Dès Noël, où un zéphyr haï me vêt de glaçons würmiens, je dîne d’exquis rôtis de bœuf au kir, à l’aÿ d’âge mûr, &cætera.'
>>> print(chr_str, len(chr_str))
Dès Noël, où un zéphyr haï me vêt de glaçons würmiens, je dîne d’exquis rôtis de bœuf au kir, à l’aÿ d’âge mûr, &cætera. 120
>>> [c for c in chr_str]
['D', 'è', 's', ' ', 'N', 'o', 'ë', 'l', ',', ' ', 'o', 'ù', ' ', 'u', 'n', ' ', 'z', 'é', 'p', 'h', 'y', 'r', ' ', 'h', 'a', 'ï', ' ', 'm', 'e', ' ', 'v', 'ê', 't', ' ', 'd', 'e', ' ', 'g', 'l', 'a', 'ç', 'o', 'n', 's', ' ', 'w', 'ü', 'r', 'm', 'i', 'e', 'n', 's', ',', ' ', 'j', 'e', ' ', 'd', 'î', 'n', 'e', ' ', 'd', '’', 'e', 'x', 'q', 'u', 'i', 's', ' ', 'r', 'ô', 't', 'i', 's', ' ', 'd', 'e', ' ', 'b', 'œ', 'u', 'f', ' ', 'a', 'u', ' ', 'k', 'i', 'r', ',', ' ', 'à', ' ', 'l', '’', 'a', 'ÿ', ' ', 'd', '’', 'â', 'g', 'e', ' ', 'm', 'û', 'r', ',', ' ', '&', 'c', 'æ', 't', 'e', 'r', 'a', '.']
[binascii.hexlify(bytearray(c, 'utf-8')) for c in chr_str]
[b'44', b'c3a8', b'73', b'20', b'4e', b'6f', b'c3ab', b'6c', b'2c', b'20', b'6f', b'c3b9', b'20', b'75', b'6e', b'20', b'7a', b'c3a9', b'70', b'68', b'79', b'72', b'20', b'68', b'61', b'c3af', b'20', b'6d', b'65', b'20', b'76', b'c3aa', b'74', b'20', b'64', b'65', b'20', b'67', b'6c', b'61', b'c3a7', b'6f', b'6e', b'73', b'20', b'77', b'c3bc', b'72', b'6d', b'69', b'65', b'6e', b'73', b'2c', b'20', b'6a', b'65', b'20', b'64', b'c3ae', b'6e', b'65', b'20', b'64', b'e28099', b'65', b'78', b'71', b'75', b'69', b'73', b'20', b'72', b'c3b4', b'74', b'69', b'73', b'20', b'64', b'65', b'20', b'62', b'c593', b'75', b'66', b'20', b'61', b'75', b'20', b'6b', b'69', b'72', b'2c', b'20', b'c3a0', b'20', b'6c', b'e28099', b'61', b'c3bf', b'20', b'64', b'e28099', b'c3a2', b'67', b'65', b'20', b'6d', b'c3bb', b'72', b'2c', b'20', b'26', b'63', b'c3a6', b'74', b'65', b'72', b'61', b'2e']
>>> chr_arr = bytearray(chr_str, 'utf-8')
>>> print(chr_arr, len(chr_arr))
bytearray(b'D\xc3\xa8s No\xc3\xabl, o\xc3\xb9 un z\xc3\xa9phyr ha\xc3\xaf me v\xc3\xaat de gla\xc3\xa7ons w\xc3\xbcrmiens, je d\xc3\xaene d\xe2\x80\x99exquis r\xc3\xb4tis de b\xc5\x93uf au kir, \xc3\xa0 l\xe2\x80\x99a\xc3\xbf d\xe2\x80\x99\xc3\xa2ge m\xc3\xbbr, &c\xc3\xa6tera.') 142
>>> chr_hex = binascii.hexlify(chr_arr)
>>> print(chr_hex, len(chr_hex))
b'44c3a873204e6fc3ab6c2c206fc3b920756e207ac3a970687972206861c3af206d652076c3aa7420646520676c61c3a76f6e732077c3bc726d69656e732c206a652064c3ae6e652064e280996578717569732072c3b47469732064652062c5937566206175206b69722c20c3a0206ce2809961c3bf2064e28099c3a26765206dc3bb722c202663c3a6746572612e' 284
```
Finally the UTF-8 encoding of a Russian [pangram][PAN_FR] containing all 33 characters of the Russian language:
```python
>>> import binascii
>>> chr_str = u'Эй, жлоб! Где туз? Прячь юных съёмщиц в шкаф'
>>> print(chr_str, len(chr_str))
Эй, жлоб! Где туз? Прячь юных съёмщиц в шкаф 44
>>> [binascii.hexlify(bytearray(c, 'utf-8')) for c in chr_str]
[b'd0ad', b'd0b9', b'2c', b'20', b'd0b6', b'd0bb', b'd0be', b'd0b1', b'21', b'20', b'd093', b'd0b4', b'd0b5', b'20', b'd182', b'd183', b'd0b7', b'3f', b'20', b'd09f', b'd180', b'd18f', b'd187', b'd18c', b'20', b'd18e', b'd0bd', b'd18b', b'd185', b'20', b'd181', b'd18a', b'd191', b'd0bc', b'd189', b'd0b8', b'd186', b'20', b'd0b2', b'20', b'd188', b'd0ba', b'd0b0', b'd184']
>>> chr_arr = bytearray(chr_str, 'utf-8')
>>> print(chr_arr, len(chr_arr))
bytearray(b'\xd0\xad\xd0\xb9, \xd0\xb6\xd0\xbb\xd0\xbe\xd0\xb1! \xd0\x93\xd0\xb4\xd0\xb5 \xd1\x82\xd1\x83\xd0\xb7? \xd0\x9f\xd1\x80\xd1\x8f\xd1\x87\xd1\x8c \xd1\x8e\xd0\xbd\xd1\x8b\xd1\x85 \xd1\x81\xd1\x8a\xd1\x91\xd0\xbc\xd1\x89\xd0\xb8\xd1\x86 \xd0\xb2 \xd1\x88\xd0\xba\xd0\xb0\xd1\x84') 77
>>> chr_hex = binascii.hexlify(chr_arr)
>>> print(chr_hex, len(chr_hex))
b'd0add0b92c20d0b6d0bbd0bed0b12120d093d0b4d0b520d182d183d0b73f20d09fd180d18fd187d18c20d18ed0bdd18bd18520d181d18ad191d0bcd189d0b8d18620d0b220d188d0bad0b0d184' 154
```
[UTF-8]: https://en.wikipedia.org/wiki/UTF-8
[PAN_DE]: https://de.wikipedia.org/wiki/Pangramm#Liste_deutscher_Pangramme
[PAN_FR]: https://fr.wikipedia.org/wiki/Pangramme#Avec_les_signes_diacritiques
[PAN_RU]: https://de.wikipedia.org/wiki/Pangramm#Russisch

## Base64 <a name="section3"></a>
The [Base64][Base64] scheme encodes `N` binary bytes into 4 * &#8968;`N/3`&#8969; printable ASCII characters and thus allows to carry data stored in binary formats across channels that only reliably support 7-bit US-ASCII text content. The encoding is shown below:

``` <!-- language: lang-none -->
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|     Binary Byte A     |     Binary Byte B     |     Binary Byte C     |
+--+--+--+---+---+---+--+--+--+--+--+---+---+---+--+--+--+--+--+--+--+--+
 A0 A1 A2 A3 A4 A5 A6 A7 B0 B1 B2 B3 B4 B5 B6 B7 C0 C1 C2 C3 C4 C5 C6 C7  Bits
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|  Base64 Char 1  |  Base64 Char 2  |  Base64 Char 3  |  Base64 Char 4  |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
```
The 24 bits `A0..A7, B0..B7, C0..C7` of 3 consecutive bytes `A`, `B` and `C` are split into 4 groups of 6 bits each that are then mapped to the 64 printable US-ASCII characters `A..Z`, `a..z`,  `0..9`,  `+` and `/` according to the table listed below.

|  I | Binary |Char|  I | Binary |Char|  I | Binary |Char|  I | Binary |Char|
|:--:|:------:|:--:|:--:|:------:|:--:|:--:|:------:|:--:|:--:|:------:|:--:|
|  0 | 000000 |  A | 16 | 010000 |  Q | 32 | 100000 |  g | 48 | 110000 |  w |
|  1 | 000001 |  B | 17 | 010001 |  R | 33 | 100001 |  h | 49 | 110001 |  x |
|  2 | 000010 |  C | 18 | 010010 |  S | 34 | 100010 |  i | 50 | 110010 |  y |
|  3 | 000011 |  D | 19 | 010011 |  T | 35 | 100011 |  j | 51 | 110011 |  z |
|  4 | 000100 |  E | 20 | 010100 |  U | 36 | 100100 |  k | 52 | 110100 |  0 |
|  5 | 000101 |  F | 21 | 010101 |  V | 37 | 100101 |  l | 53 | 110101 |  1 |
|  6 | 000110 |  G | 22 | 010110 |  W | 38 | 100110 |  m | 54 | 110110 |  2 |
|  7 | 000111 |  H | 23 | 010111 |  X | 39 | 100111 |  n | 55 | 110111 |  3 |
|  8 | 001000 |  I | 24 | 011000 |  Y | 40 | 101000 |  o | 56 | 111000 |  4 |
|  9 | 001001 |  J | 25 | 011001 |  Z | 41 | 101001 |  p | 57 | 111001 |  5 |
| 10 | 001010 |  K | 26 | 011010 |  a | 42 | 101010 |  q | 58 | 111010 |  6 |
| 11 | 001011 |  L | 27 | 011011 |  b | 43 | 101011 |  r | 59 | 111011 |  7 |
| 12 | 001100 |  M | 28 | 011100 |  c | 44 | 101100 |  s | 60 | 111100 |  8 |
| 13 | 001101 |  N | 29 | 011101 |  d | 45 | 101101 |  t | 61 | 111101 |  9 |
| 14 | 001110 |  O | 30 | 011110 |  e | 46 | 101110 |  u | 62 | 111110 |  + |
| 15 | 001111 |  P | 31 | 011111 |  f | 47 | 101111 |  v | 63 | 111111 |  / |

If the total number  `N` of bytes contained in a binary blob is not an exact multiple of three, i.e. `N mod 3 != 0` then one or two bytes remain at the end of the data block which have to be processed separately.:

For the case `N mod 3 = 2`,  the remaining 16 bits `A0..A7, B0..B7` are padded by two zero bits and the resulting 18 bits can then be mapped to 3 Base64 characters. In order to indicate the padding, an additional `=` ASCII character is appended, making the total number of Base64 characters a multiple of four.
``` <!-- language: lang-none -->
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|     Binary Byte A     |     Binary Byte B     |
+--+--+--+---+---+---+--+--+--+--+--+---+---+---+
 A0 A1 A2 A3 A4 A5 A6 A7 B0 B1 B2 B3 B4 B5 B6 B7  0  0                    Bits
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|  Base64 Char 1  |  Base64 Char 2  |  Base64 Char 3  |        =        |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
```

For the case `N mod 3 = 1`,  the remaining 8 bits `A0..A7` are padded by four zero bits and the resulting 12 bits can then be mapped to 2 Base64 characters. In order to indicate the padding, two additional `=` ASCII characters are appended, making the total number of Base64 characters a multiple of four.
``` <!-- language: lang-none -->
+--+--+--+--+--+--+--+--+
|     Binary Byte A     |
+--+--+--+---+---+---+--+
 A0 A1 A2 A3 A4 A5 A6 A7 0  0  0  0                                       Bits
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|  Base64 Char 1  |  Base64 Char 2  |        =        |        =        |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
```

**Python 3**: <a name="python3"></a> Use the built-in Base64 encoding and decoding functions and compare it to Hex encoding.

Base64 encoding of 6 binary bytes:
```python
>>> import base64, binascii
>>> data_bin = bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66])
>>> [format(x, '08b') for x in data_bin]
['00010001', '00100010', '00110011', '01000100', '01010101', '01100110']
>>> data_hex = binascii.hexlify(data_bin)
>>> print(data_hex, len(data_hex))
b'112233445566' 12
>>> data_b64 = base64.b64encode(data_bin)
>>> print(data_b64, len(data_b64))
b'ESIzRFVm' 8
>>> [format(x, '08b') for x in base64.b64decode(data_b64)]
['00010001', '00100010', '00110011', '01000100', '01010101', '01100110']
```
Base64 encoding of 5 binary bytes requires padding:
```python
>>> import base64, binascii
>>> data_bin = bytes([0x11, 0x22, 0x33, 0x44, 0x55])
>>> [format(x, '08b') for x in data_bin]
['00010001', '00100010', '00110011', '01000100', '01010101']
>>> data_hex = binascii.hexlify(data_bin)
>>> print(data_hex, len(data_hex))
b'1122334455' 10
>>> data_b64 = base64.b64encode(data_bin)
>>> print(data_b64, len(data_b64))
b'ESIzRFU=' 8
>>> [format(x, '08b') for x in base64.b64decode(data_b64)]
['00010001', '00100010', '00110011', '01000100', '01010101']
```
Base64 encoding of 4 binary bytes requires padding:
```python
>>> import base64, binascii
>>> data_bin = bytes([0x11, 0x22, 0x33, 0x44])
>>> [format(x, '08b') for x in data_bin]
['00010001', '00100010', '00110011', '01000100']
>>> data_hex = binascii.hexlify(data_bin)
>>> print(data_hex, len(data_hex))
b'11223344' 8
>>> data_b64 = base64.b64encode(data_bin)
>>> print(data_b64, len(data_b64))
b'ESIzRA==' 8
>>> [format(x, '08b') for x in base64.b64decode(data_b64)]
['00010001', '00100010', '00110011', '01000100']
```

[Base64]: https://en.wikipedia.org/wiki/Base64

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/

