# Capture the Flag 2

What secret does the big bad wolf carry in its belly?

![Little Red Riding Hood][CTF2_IMG]

Here is the listing of the C language program `wolf.c` that in turn calls the function `belly()` whose content is unknown.
```C
#include "belly.h"

#include <stdint.h>

void main(int argc, char* argv[])
{
    uint64_t a = 0x1122334455667788;
    uint8_t s = 2;
    uint32_t d;

    d = belly(a, s);
}
```
Any secrets you may find in the binary object [wolf][CTF2_OBJ] are encapsulated within curly brackets `{secret}`. 

[CTF2_IMG]: https://upload.wikimedia.org/wikipedia/commons/thumb/b/b4/Little_Red_Riding_Hood_-_J._W._Smith.jpg/417px-Little_Red_Riding_Hood_-_J._W._Smith.jpg "CC0"
[CTF2_OBJ]: wolf

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
