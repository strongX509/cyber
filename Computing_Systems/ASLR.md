# Address Space Layout Randomization

## Table of Contents
1. [Randomizing Stack and Heap](#section1)
2. [Changing ASLR Levels](#section2)
3. [Position Independent Executable](#section3)
4. [Canaries](#section4)

C language programs: &nbsp;[C1](#c1)

## Randomizing Stack and Heap <a name="section1"></a>

In order to thwart an unfriendly takeover of a running program by malware via a [Buffer Overflow][BUF_OVERFLOW] attack, all modern operating systems have the option to randomize the virtual memory locations of the *Stack* and *Heap* segments of a process.
This technique is called *Address Space Layout Randomization (ASLR)*.

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
12     strcpy(buf_stack, b);
13     strcpy(buf_heap,  b);
14
15     uint64_t canary = *(uint64_t*) (buf_stack +  8);
16     uintptr_t rbp   = *(uintptr_t*)(buf_stack + 16);
17     uintptr_t rip   = *(uintptr_t*)(buf_stack + 24);
18
19     printf("heap 0x%012" PRIx64 " stack %p cny 0x%016" PRIx64
20            " rbp 0x%012" PRIx64 " rip 0x%012" PRIx64 "\n",
21            (uintptr_t)buf_heap, buf_stack, canary, rbp, rip);
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
> gcc -ggdb -fstack-protector-strong -o aslr aslr.c
> gdb aslr
```
We set a breakpoint on line 19 right before the `printf` statement in function `copy` and run the program with the command line argument `argv[1] = "1234567"`  which with the additional `nul` character terminating the string just fills the 8 byte deep stack and heap buffers.
```assembly
(gdb) break 19
Breakpoint 1 at 0x83f: file aslr.c, line 19.
(gdb) run "1234567"
Starting program: /home/andi/cyber/Computing_Systems/aslr "1234567"
Breakpoint 1, copy (b=0x7fffffffe1d8 "1234567") at aslr.c:19
19	    printf("heap 0x%012" PRIx64 " stack %p cny 0x%016" PRIx64
```
Now let's have a look at the frame information within the `copy` function which tells us the position of the saved `rbp` and `rip` registers on the stack:
```assembly
(gdb) info frame
Stack level 0, frame at 0x7fffffffdce0:
 rip = 0x55555555483f in copy (aslr.c:19); saved rip = 0x5555555548b9
 called by frame at 0x7fffffffdd10
 source language c.
 Arglist at 0x7fffffffdcd0, args: b=0x7fffffffe1d8 "1234567"
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
(gdb) print/x $rbp
$8 = 0x7fffffffdcd0
```
The last `print` command  above shows the current stack pointer  to be at `0x7fffffffdc90` and the `info frame` output tells the previous frame `sp` to be at `0x7fffffffdce0`. Thus the local stack of the function `copy` spans the following range of 80 bytes:
```assembly
(gdb) x/80xb 0x7fffffffdc90
0x7fffffffdc90: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00  # <- current rsp
0x7fffffffdc98: 0xd8 0xe1 0xff 0xff 0xff 0x7f 0x00 0x00  # function argument b
0x7fffffffdca0: 0x60 0x62 0x75 0x55 0x55 0x55 0x00 0x00  # local var buf_heap
0x7fffffffdca8: 0x00 0x1b 0x6c 0xb9 0xf1 0x26 0x09 0xa7  # local var canary
0x7fffffffdcb0: 0x00 0xdd 0xff 0xff 0xff 0x7f 0x00 0x00  # local var rbp
0x7fffffffdcb8: 0xb9 0x48 0x55 0x55 0x55 0x55 0x00 0x00  # local var rip
0x7fffffffdcc0: 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x00  # local var buf_stack
0x7fffffffdcc8: 0x00 0x1b 0x6c 0xb9 0xf1 0x26 0x09 0xa7  # canary
0x7fffffffdcd0: 0x00 0xdd 0xff 0xff 0xff 0x7f 0x00 0x00  # saved rbp  <- current rbp
0x7fffffffdcd8: 0xb9 0x48 0x55 0x55 0x55 0x55 0x00 0x00  # saved rip
```
When we continue the execution of the program, the position of the local variable  `buf_stack` as well as the contents of the local variables `buf_heap`, `canary`,  `rbp` and `rip` are printed.  The print output can be compared with the stack dump listed above, taking into account that 64 bit pointers and integers are stored in *little-endian* host-order format. 

```assembly
(gdb) continue
Continuing.
heap 0x555555756260 stack 0x7fffffffdcc0 cny 0xa70926f1b96c1b00 rbp 0x7fffffffdd00 rip 0x5555555548b9
[Inferior 1 (process 4387) exited normally]
(gdb) quit
```
Now we run the program a second time in the debugger without setting any breakpoints and note that the virtual memory position of the *Heap* and *Stack* variables as well as the program instruction addresses   remain the same which tremendously facilitates debugging.  Only the random  *Canary* field inserted by the compiler to prevent the overwriting of the return address saved on the stack is changing.
```assembly
> gdb aslr
(gdb) run "1234567"
Starting program: /home/andi/cyber/Computing_Systems/aslr "1234567"
Starting program: /home/andi/Documents/Cyber RS/cyber/Computing_Systems/aslr "1234567"
heap 0x555555756260 stack 0x7fffffffdcc0 cny 0xd365d6329b0d6c00 rbp 0x7fffffffdd00 rip 0x5555555548b9
[Inferior 1 (process 4882) exited normally]
(gdb) quit
```
Examining the virtual stack and heap addresses of the string buffers as well as the  saved instruction pointer, we experience a big surprise when we run the program several times outside the debugger environment:
```console
> for i in {1..10} ; do ./aslr "1234567" ; done
heap 0x55738d2a7260 stack 0x7ffe6a0674d0 cny 0xcbaae0af2f748a00 rbp 0x7ffe6a067510 rip 0x55738b0aa8b9
heap 0x55a658e4f260 stack 0x7ffde509c4b0 cny 0xcb34626fdd71b200 rbp 0x7ffde509c4f0 rip 0x55a656dcd8b9
heap 0x562f9cf03260 stack 0x7ffda5c0db10 cny 0xca82ebfe72303100 rbp 0x7ffda5c0db50 rip 0x562f9cb3a8b9
heap 0x5635bf6d4260 stack 0x7ffde753ef20 cny 0x250d5817f3ca1900 rbp 0x7ffde753ef60 rip 0x5635bdf378b9
heap 0x55d21cbdc260 stack 0x7fff6a0997b0 cny 0xe64e39d808fcca00 rbp 0x7fff6a0997f0 rip 0x55d21b1f78b9
heap 0x55e178fd2260 stack 0x7fffc747f790 cny 0xfae4954145520700 rbp 0x7fffc747f7d0 rip 0x55e1784c88b9
heap 0x5574bc456260 stack 0x7ffca42605f0 cny 0x51d553c639f92a00 rbp 0x7ffca4260630 rip 0x5574bb7c28b9
heap 0x55ecedd7c260 stack 0x7fffcbed62e0 cny 0xe3106bd01662a300 rbp 0x7fffcbed6320 rip 0x55eced9e68b9
heap 0x55e250ccd260 stack 0x7ffe283b2230 cny 0x129819a9a45ad100 rbp 0x7ffe283b2270 rip 0x55e24f29d8b9
heap 0x560f373c1260 stack 0x7ffec07ed370 cny 0xb7096d4ab04df500 rbp 0x7ffec07ed3b0 rip 0x560f366378b9
```
By running `aslr` several thousand times we can experimentally determine the highest and lowest virtual memory locations of the *Text*, *Heap* and *Stack* segments.

|       |             Text |             Heap |            Stack |               Canary |
| :---- | ---------------: | ---------------: | ---------------: | -------------------: |
| Max   | `0x56554deff8b9` | `0x56554fa96260` | `0x7fffffdfee10` | `0xffffacbf2a69be00` |
| Min   | `0x5555584688b9` | `0x555557363260` | `0x7ffc0000b0f0` | `0x00000871df3f4200` |
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

Modern Linux distributions set ASLR level 2 by default:
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
heap 0x555555756260 stack 0x7fffffffdd60 cny 0xb55d493f4abbca00 rbp 0x7fffffffdda0 rip 0x5555555548b9
heap 0x555555756260 stack 0x7fffffffdd60 cny 0x42adb0e8d2d23e00 rbp 0x7fffffffdda0 rip 0x5555555548b9
```

## Position-Independent Executable <a name="section3"></a>

A [Position-Independent Executable][PIE] (PIE) is a piece of machine code that can be executed anywhere in primary memory regardless of its *absolute address*. PIE-enabled code is thus the prerequisite for the address randomization of the *Text* virtual memory segment.  The `-pie` option of the `gcc` compiler produces position-independent code. On the *Ubuntu 18.04* Linux platform this tutorial was developed on, `-pie` is enabled by default. This is the reason that we noticed the randomization of the *Text* segment with ASLR level 2 in the previous section. Therefore we are now going to show how non-relocatable machine code looks like by explicitly disabling PIE with the `-no-pie` compiler option.
```console
> gcc -ggdb -fstack-protector-strong -no-pie -o aslr_no_pie aslr.c
```
With the following `sysctl` command we chan check that we are still on ASLR level 0:
```console
> sysctl -a --pattern randomize
kernel.randomize_va_space = 0
```
Next we call `aslr_no_pie` twice and see that ASLR has in fact been disabled and that the virtual memory location of the *Text* and *Heap* segments is very low at `0x00..` instead of `0x55..`
```console
> for i in {1..2} ; do ./aslr_no_pie "1234567" ; done
heap 0x000000602260 stack 0x7fffffffdd40 cny 0xce0347ab0d5b4e00 rbp 0x7fffffffdd80 rip 0x000000400766
heap 0x000000602260 stack 0x7fffffffdd40 cny 0x1e785467410fc300 rbp 0x7fffffffdd80 rip 0x000000400766
```
We enable full randomization by going to ASLR level 2 again using the `sysctl` command
```console
> sudo sysctl -w kernel.randomize_va_space=2
kernel.randomize_va_space = 2
```
Now we see that the *Heap*  segment is randomized around its low `0x00..` virtual memory position whereas the non-PIE *Text* segment cannot be randomized
```console
> for i in {1..2} ; do ./aslr_no_pie "1234567" ; done
heap 0x0000006e9260 stack 0x7fffdd5e4a00 cny 0xebbe47a35d46a700 rbp 0x7fffdd5e4a40 rip 0x000000400766
heap 0x000001119260 stack 0x7ffc1c296a90 cny 0x03aafc45c6f55900 rbp 0x7ffc1c296ad0 rip 0x000000400766
```

[PIE]: https://en.wikipedia.org/wiki/Position-independent_code

## Canaries <a name="section4"></a>

Canary birds were historically used in coal mines as an early warning system  since they would be affected by toxic gases earlier than the miners and fall off the perch in their cage. In software [Canaries][CANARIES] are known values that are placed between a buffer and control data on the stack to monitor buffer overflows. When the buffer overflows, the first data to be corrupted will usually be the canary and a failed verification of the canary data will therefore generate an exception and abort the running program.

We can see the canary mechanism in action when we produce a buffer overflow on the stack by incrementally increasing the length of the command line input argument `argv[1]`:
```console
> ./aslr "12345678"
heap 0x5626197c0260 stack 0x7ffd0b53f090 cny 0x3424fa0e02f75500 rbp 0x7ffd0b53f0d0 rip 0x5626183878b9
```
The ninth character of the input string is the terminating `nul` character which overwrites the first character of the 64 bit canary word. Nothing happens because the LSB of a canary is always a `nul` character. But when we increase the input string to 10 characters:
```console
> ./aslr "123456789"
heap 0x557e75a1d260 stack 0x7fff71018e40 cny 0xa40367758a7c0039 rbp 0x7fff71018e80 rip 0x557e73a408b9
*** stack smashing detected ***: <unknown> terminated
Aborted (core dumped)
```
the program flow is aborted because the first random byte of the canary is overwritten by the `nul` character and the change is detected by the following machine instructions inserted by the compiler
at the very end of the `copy` function just before returning to the main program as the following snippet from the disassembled `copy` machine code shows:

```assembly
0x0000555555554871 <+167>: mov    -0x8(%rbp),%rcx            ; copy current canary to %rcx
0x0000555555554875 <+171>: xor    %fs:0x28,%rcx              ; xor with original canary
0x000055555555487e <+180>: je     0x555555554885 <copy+187>  ; if equal 0 jump to <+187>
0x0000555555554880 <+182>: callq  0x555555554670 <__stack_chk_fail@plt>  ; exception
0x0000555555554885 <+187>: leaveq                            ; recover %rsp and %rbp of main
0x0000555555554886 <+188>: retq                              ; return to main
```
A thread-specific random canary is generated by the Linux operating system and stored globally at a memory location pointed to by  `%fs:0x28`. The  memory locations managed by the special `%fs` register cannot be modified by a user process, so that the canary value cannot be tampered with by malware.

When the function `copy` is entered from the main program the global canary is copied from `%fs:0x28`  to the local stack just below the saved `%rbp` and`%rip` registers, thus protecting them from change:

```assembly
0x00005555555547ca <+0>:   push   %rbp                       ; save %rbp of main
0x00005555555547cb <+1>:   mov    %rsp,%rbp                  ; current %rsp becomes %rbp
0x00005555555547ce <+4>:   sub    $0x40,%rsp                 ; lower %rsp by 64 bytes
0x00005555555547d2 <+8>:   mov    %rdi,-0x38(%rbp)
0x00005555555547d6 <+12>:  mov    %fs:0x28,%rax              ; copy canary to %rax
0x00005555555547df <+21>:  mov    %rax,-0x8(%rbp)            ; copy canary below saved %rbp
```
The `-fstack-protector-strong` option of the `gcc` compiler which is set by default e.g. by *Ubuntu 18.04*, automatically inserts the canary code above when character buffers are used in functions.  The  stack protection can be disabled with the `-fno-stack-protector` option.

[CANARIES]: https://en.wikipedia.org/wiki/Buffer_overflow_protection#Canaries

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
