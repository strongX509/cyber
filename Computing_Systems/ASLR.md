# Address Space Layout Randomization

## Table of Contents
1. [Randomizing Stack and Heap](#section1)
2. [Changing ASLR Levels](#section2)

C language programs: &nbsp;[C1](#c1)

## Randomizing Stack and Heap <a name="section1"></a>

In order to thwart an unfriendly takeover of a running program by malware via a [Buffer Overflow][BUF_OVERFLOW] attack, all modern operating systems have the option to randomize the virtual memory locations of the *Stack* and *Heap* segments of a process.

**C 1**: <a name="c1"></a> The C program [aslr.c](aslr.c) consists of a main program `main()` that calls the function `copy()` which puts a copy of the command line argument string `argv[1]` both on the *Stack* and the *Heap* by using the very dangerous `strcpy()` libc system function which does not do any array size checking:

```C
 1 #include <string.h>
 2 #include <stdio.h>
 3 #include <stdlib.h>
 4 #include <stdint.h>
 5 #include <inttypes.h>
 6
 7 char* copy(char *b)
 8 {
 9     char  buf_stack[8];
10     char *buf_heap = malloc(8);
11
12     uint64_t canary = *(uint64_t*)(buf_stack +  8);
13     intptr_t rbp    = *(intptr_t*)(buf_stack + 16);
14     intptr_t rip    = *(intptr_t*)(buf_stack + 24);
15
16     strcpy(buf_stack, b);
17     strcpy(buf_heap,  b);
18
19     printf("heap %p stack %p cny 0x%016" PRIx64 " rbp %p rip %p\n",
20             buf_heap, buf_stack, canary, (void*)rbp, (void*)rip);
21
22     return buf_heap;   
23 }
24
25 int main(int argc, char** argv)
26 {
27     char *b;
28
29     if (argc < 2)
30     {
31         exit(1);
32     }
33     b = copy(argv[1]);
34
35     free(b);
36     exit(0);
37 }
```
We compile the program with the use of frame pointers and first start the binary in the debugger
```console
> gcc -ggdb -o aslr aslr.c
> gdb aslr
```
We set a breakpoint on line 19 right after the `strcpy` operation in function `copy` and run the program with the command line argument `argv[1] = "1234567"`  which with the additional `nul` character terminating the string just fills the 8 byte deep stack and heap buffers.
```assembly
(gdb) (gdb) break 19
Breakpoint 1 at 0x831: file aslr.c, line 19.
(gdb) run "1234567"
Starting program: /home/andi/cyber/Computing_Systems/aslr "1234567"
Breakpoint 1, copy (b=0x7fffffffe1d9 "1234567") at aslr.c:19
19	    printf("heap %p stack %p cny 0x%016" PRIx64 " rbp %p rip %p\n",
```
Now let's have a look at the frame information within the `copy` function which tells us the position of the saved `rbp` and `rip` registers on the stack:
```assembly
(gdb) info frame
Stack level 0, frame at 0x7fffffffdce0:
 rip = 0x555555554831 in copy (aslr.c:19); saved rip = 0x5555555548ab
 called by frame at 0x7fffffffdd10
 source language c.
 Arglist at 0x7fffffffdcd0, args: b=0x7fffffffe1d9 "1234567"
 Locals at 0x7fffffffdcd0, Previous frame's sp is 0x7fffffffdce0
 Saved registers:
  rbp at 0x7fffffffdcd0, rip at 0x7fffffffdcd8
```
Additionally we determine the position of the local variables and the function parameter `b` on the stack
```assembly
(gdb) print/x &buf_stack
$1 = 0x7fffffffdcc0
(gdb) print/x &rip
$2 = 0x7fffffffdcb8
(gdb) print/x &rbp
$3 = 0x7fffffffdcb0
(gdb) print/x &canary
$4 = 0x7fffffffdca8
(gdb) print/x &buf_heap
$5 = 0x7fffffffdca0
(gdb) print/x &b
$6 = 0x7fffffffdc98
(gdb) print/x $rsp
$7 = 0x7fffffffdc90
```
The last `print` command  above shows the current stack pointer  to be at `0x7fffffffdc90` and the `info frame` output tells the previous frame `sp` to be at `0x7fffffffdce0`. Thus the local stack of the function `copy` spans the following range of 80 bytes:
```assembly
(gdb) x/80xb 0x7fffffffdc80
0x7fffffffdc80: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00  # <- current sp
0x7fffffffdc88: 0xbc 0xe1 0xff 0xff 0xff 0x7f 0x00 0x00  # function argument b
0x7fffffffdc90: 0x60 0x62 0x75 0x55 0x55 0x55 0x00 0x00  # local var buf_heap
0x7fffffffdc98: 0x00 0x0b 0xca 0xe0 0xe4 0x56 0x42 0xde  # local var canary
0x7fffffffdca0: 0xf0 0xdc 0xff 0xff 0xff 0x7f 0x00 0x00  # local var rbp
0x7fffffffdca8: 0xab 0x48 0x55 0x55 0x55 0x55 0x00 0x00  # local var rip
0x7fffffffdcb0: 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x00  # local var buf_stack
0x7fffffffdcb8: 0x00 0x0b 0xca 0xe0 0xe4 0x56 0x42 0xde  # canary
0x7fffffffdcc0: 0xf0 0xdc 0xff 0xff 0xff 0x7f 0x00 0x00  # saved rbp
0x7fffffffdcc8: 0xab 0x48 0x55 0x55 0x55 0x55 0x00 0x00  # saved rip
```
When we continue the execution of the program, the position of the local variable  `buf_stack` as well as the contents of the local variables `buf_heap`, `canary`,  `rbp` and `rip` are printed.  The print output can be compared with the stack dump listed above, taking into account that 64 bit pointers and integers are stored in *little-endian* host-order format. 

```assembly
(gdb) continue
Continuing.
heap 0x555555756260 stack 0x7fffffffdcc0 cny 0xa771f7e56a87f100 rbp 0x7fffffffdd00 rip 0x5555555548ab
[Inferior 1 (process 5632) exited normally]
(gdb) quit
```
Now we run the program a second time in the debugger without setting any breakpoints and note that the virtual memory position of the *Heap* and *Stack* variables as well as the program instruction addresses   remain the same which tremendously facilitates debugging.  Only the random  *Canary* field inserted by the compiler to prevent the overwriting of the return address saved on the stack is changing.
```assembly
> gdb aslr
(gdb) run "1234567"
Starting program: /home/andi/cyber/Computing_Systems/aslr "1234567"
heap 0x555555756260 stack 0x7fffffffdcc0 cny 0x8edc54f6489c6300 rbp 0x7fffffffdd00 rip 0x5555555548ab
[Inferior 1 (process 6392) exited normally]
(gdb) quit
```
Examining the virtual stack and heap addresses of the string buffers as well as the  saved instruction pointer, we experience a big surprise when we run the program several times outside the debugger environment:
```console
> for i in {1..10} ; do ./aslr "1234567" ; done
heap 0x564b4549e260 stack 0x7ffc57d280d0 cny 0xeb492b6b41da2400 rbp 0x7ffc57d28110 rip 0x564b436568ab
heap 0x55f8954a0260 stack 0x7ffd62dbbc90 cny 0xebeda540cad34800 rbp 0x7ffd62dbbcd0 rip 0x55f894aa08ab
heap 0x564d8ee63260 stack 0x7ffc6e5df8a0 cny 0x67aeb25cf0ca1200 rbp 0x7ffc6e5df8e0 rip 0x564d8e64d8ab
heap 0x55d923bf2260 stack 0x7fff394ae1b0 cny 0xdf89c03271497100 rbp 0x7fff394ae1f0 rip 0x55d92204f8ab
heap 0x558d52889260 stack 0x7ffc1e6d5cb0 cny 0x5a40abfd5e1b7600 rbp 0x7ffc1e6d5cf0 rip 0x558d521938ab
heap 0x5613e34eb260 stack 0x7ffca6f817e0 cny 0xcfcd257c1be55900 rbp 0x7ffca6f81820 rip 0x5613e1af78ab
heap 0x564cd7374260 stack 0x7fff82a656b0 cny 0x072c237fe96d1a00 rbp 0x7fff82a656f0 rip 0x564cd57698ab
heap 0x560c148e2260 stack 0x7ffdd376aa10 cny 0x7c7fb34cd4d5a100 rbp 0x7ffdd376aa50 rip 0x560c13bbd8ab
heap 0x561e86e68260 stack 0x7ffdd1805dc0 cny 0x4869c8299099b400 rbp 0x7ffdd1805e00 rip 0x561e850538ab
heap 0x555fde0ad260 stack 0x7fffb85b10c0 cny 0xa60c29d806fd2200 rbp 0x7fffb85b1100 rip 0x555fdd01a8ab

```
By running `aslr` several thousand times we can experimentally determine the highest and lowest virtual memory locations of the *Text*, *Heap* and *Stack* segments.

|       |             Text |             Heap |            Stack |               Canary |
| :---- | ---------------: | ---------------: | ---------------: | -------------------: |
| Max   | `0x56554deff8ab` | `0x56554fa96260` | `0x7fffffdfee10` | `0xffffacbf2a69be00` |
| Min   | `0x5555584688ab` | `0x555557363260` | `0x7ffc0000b0f0` | `0x00000871df3f4200` |
| Diff  | `0x00fff5a97000` | `0x00fff8733000` | `0x0003ffdf3d20` | `0xffffa44d4b2a7c00` |
| Range | `0x010000000000` | `0x010000000000` | `0x000400000000` | `0x..............00` |
| Bytes |      `2^40` &#8776; 1T |      `2^40` &#8776; 1T |     `2^34` &#8776; 16G |         `2^56` &#8776; 64P |
| Step  |         `0x1000` |         `0x1000` |           `0x10` |              `0x100` |
| Bytes |             4096 |             4096 |               16 |                  256 |

Thus the virtual memory location of the *Stack* is varying in a range of about `2^34` bytes corresponding to 16 Gbytes with a resolution of  16 bytes resulting in `2^30` or about 1 Giga possible stack positions.

The virtual memory locations of the  *Heap* and *Text* segments are varying in a range of about `2^40` bytes corresponding to 1 Tbytes with a resolution of the standard Linux memory page size of 4096 bytes resulting in `2^28` or about 256 Mega possible heap positions.

[BUF_OVERFLOW]: https://en.wikipedia.org/wiki/Buffer_overflow

## Changing ASLR Levels <a name="section2"></a>

Linux ASLR can be configured through setting a flag in `/proc/sys/kernel/randomize_va_space`. Three levels are available:

`0` – No randomization. Everything is static.

`1` – Conservative randomization. Shared libraries, stack  and heap are randomized.

`2` – Full randomization. Memory managed through brk() is also randomized.

Modern Linux kernels use ASLR level 2 by default:
```console
> cat /proc/sys/kernel/randomize_va_space
2
```
If we completely disable ASLR as `root` with
```console
> sudo -s
> echo 0 > /proc/sys/kernel/randomize_va_space
```
then always the same static virtual memory map is used.  If  the program is run outside the debugger then the value of the saved `rbp`  and correspondingly the location of the local `buf_stack` variable is higher because `gdb` uses some stack space itself before `aslr` is run.
```console
> for i in {1..2} ; do ./aslr "1234567" ; done
heap 0x555555756260 stack 0x7fffffffe3d0 cny 0xcc2607f8089f0200 rbp 0x7fffffffe410 rip 0x5555555548ab
heap 0x555555756260 stack 0x7fffffffe3d0 cny 0xb70550f68b9f7700 rbp 0x7fffffffe410 rip 0x5555555548ab
```

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
