# Capture the Flag 2

What secret does the big bad wolf carry in its belly?

![Little Red Riding Hood][CTF2_IMG]

Here is the listing of the C language program `wolf.c` that in turn calls the function `belly()` whose content is unknown.
```C
 1 #include "belly.h"
 2
 3 #include <stdint.h>
 4
 5 void main(int argc, char* argv[])
 6 {
 7     uint64_t a = 0x1122334455667788;
 8     uint8_t s = 2;
 9     uint32_t d;
10
11     d = belly(a, s);
12 }
```
Any secrets you may find in the binary object [wolf][CTF2_OBJ] are encapsulated within curly brackets `{secret}`. 

[CTF2_IMG]: https://upload.wikimedia.org/wikipedia/commons/thumb/b/b4/Little_Red_Riding_Hood_-_J._W._Smith.jpg/417px-Little_Red_Riding_Hood_-_J._W._Smith.jpg "CC0"
[CTF2_OBJ]: wolf

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
