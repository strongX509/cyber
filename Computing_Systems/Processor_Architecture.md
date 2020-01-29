

# Processor Architecture

## Table of Contents
1. [Architecture Overview](#section1)
4. [The x86-64 Registers](#section2)
3. [The Stack](#section3)
4. [Code Optimization](#section4)
5. [The Heap](#section5)

C language programs: &nbsp;[C1](#c1)  &nbsp;[C3](#c3)

Assembler exercises: &nbsp; [ASM1](#asm1) &nbsp; [ASM2](#asm2) &nbsp; [ASM3](#asm3)

##  Architecture Overview <a name="section1"></a>

The general architecture of a Central Processing Unit (CPU) is shown in the diagram below:
![Processor Architecture][PROC_ARCH_IMG]

[PROC_ARCH_IMG]: Processor_600.png "Courtesy of Phil Janson"

* The program is stored in the form of binary machine instructions in the *Program Memory* which is usually attached externally to the *Integrated Circuit* containing the *Centrol Processing Unit* (CPU).
* The program flow is controlled by the *Instruction Counter* which in normal mode proceeds linearly, i.e. with increasing instruction addresses through the program memory.
* Each binary instruction addressed in the program memory by the instruction counter is split by the *Decoder* into its components: The *Op Code* like `add` or `mov` itself and optional  *Source* and *Destination* storage locations which can be either *Registers* for fast access or addresses of data words that have to be fetched from external *Data Memory*, first.
* Data memory can be further split into the *Stack* which temporarily stores the *Local Variables* of functions and the *Heap* which allocates *Dynamic Memory* requested e.g. by the [malloc()][MAN_MALLOC] system command.
* Instructions doing some computations on its input data make use of the [Arithmetic Logic Unit][ALU] (ALU) that performs *arithmetic* and *bitwise* operations on integer binary numbers in hardware. *Floating Point* operations are usually done by a special [Floating Point Unit][FPU] (FPU).
* Instruction executing a `jump` operation either *unconditionally* or based on a preceding `cmp` or `test` *comparison* operation overwrite the instruction counter with the *destination jump address*. Also the `call` and `ret` instructions used to enter and leave *Functions* change the instruction counter.

[MAN_MALLOC]: http://man7.org/linux/man-pages/man3/malloc.3.html
[ALU]: https://en.wikipedia.org/wiki/Arithmetic_logic_unit
[FPU]: https://en.wikipedia.org/wiki/Floating-point_unit

##  The x86-64 Registers <a name="section2"></a>

The  [x86_64][X86_CS] processor architecture has sixteen 64-bit registers that may also be accessed as 32-, 16-, or 8-bit registers. The register names are as follows:

|64 bit (bytes 0..7)|32 bit (bytes 0..3)|16 bit (bytes 0..1)|8 bit (byte 0)|function calls|
|:-----:|:-----:|:-----:|:-----:|:--------------|
| %rax  | %eax  | %ax   | %al   | return value  |
| %rcx  | %ecx  | %cx   | %cl   | parameter 4   |
| %rdx  | %edx  | %dx   | %dl   | parameter 3   |
| %rbx  | %ebx  | %bx   | %bl   | callee-saved  |
| %rsi  | %esi  | %si   | %sil  | parameter 2   |
| %rdi  | %edi  | %di   | %dil  | parameter 1   |
| %rsp  | %esp  | %sp   | %spl  | stack pointer |
| %rbp  | %ebp  | %bp   | %bpl  | callee-saved* |
| %r8   | %r8d  | %r8w  | %r8b  | parameter 5   |
| %r9   | %r9d  | %r9w  | %r9b  | parameter 6   |
| %r10  | %r10d | %r10w | %r10b |               |
| %r11  | %r11d | %r11w | %r11b |               |
| %r12  | %r12d | %r12w | %r12b | callee-saved  |
| %r13  | %r13d | %r13w | %r13b | callee-saved  |
| %r14  | %r14d | %r14w | %r14b | callee-saved  |
| %r15  | %r15d | %r15w | %r15b | callee-saved  |

\* *%rbp is optionally used as base or frame pointer (disabled by -fomit-frame-pointer gcc option)*

[X86_CS]: https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf

## The Stack <a name="section3"></a>

The next C program example is used to show the allocation of local variables on the processor stack.

**C 1**: <a name="c1"></a> The C file [incr.c](incr.c) implements a function `incr()` which increments its unsigned integer arguments `v64`, `v32`,  `v16` and `v8` by one and returns the results in *host order* packed into a byte arrary `buf`:

```C
 1 #include "incr.h"
 2
 3 void incr(uint64_t v64, uint32_t v32, uint16_t v16, uint8_t v8, uint8_t buf[])
 4 {
 5     /* increment inputs by one */
 6     v64 += 1;
 7     v32 += 1;
 8     v16 += 1;
 9     v8  += 1;
10
11     /* cast results into buffer */
12     *((uint64_t*)&buf[ 0]) = v64;
13     *((uint32_t*)&buf[ 8]) = v32;
14     *((uint16_t*)&buf[12]) = v16;
15     *((uint8_t*) &buf[14]) = v8;
16 }
```
This function `incr()` is called by a main program [ctypes.c](ctypes.c)
```C
 1 #include <stdlib.h>
 2 #include <stdio.h>
 3
 4 #include "incr.h"
 5
 6 int main(int argc, char** argv)
 7 {
 8     uint64_t v64 = 0x0011223344556677;
 9     uint32_t v32 = 0x8899aabb;
10     uint16_t v16 = 0xccdd;
11     uint8_t  v8  = 0xee;
12     uint8_t buf[16];
13     const int len = sizeof(uint64_t) + sizeof(uint32_t) +
14                     sizeof(uint16_t) + sizeof(uint8_t);
15     int i;
16
17     incr(v64, v32, v16, v8, buf);
18
19     printf("buf = 0x");
20     for (i = 0; i < len; i++)
21     {
22         printf("%02x", buf[i]);
23     }
24     printf("\n");
25     exit(0);
26 }
```
In order for the main program to be able to call the external function `incr()`,  the function interface must be known by `ctypes.c`. This is done by including the header file [incr.h](incr.h).
```C
 1 #ifndef INCR_H_
 2 #define INCR_H_
 3
 4 #include <stdint.h>
 5
 6 /**
 7  * Increments each of the inputs v64, v32, v16, and v8 by one and returns
 8  * the results in host order packed into buf
 9  */
10 void incr(uint64_t v64, uint32_t v32, uint16_t v16, uint8_t v8, uint8_t buf[]);
11
12 #endif /** INCR_H_ */
```
The standard unsigned integer types are defined by the included header file `stdint.h` and the `#ifndef` C preprocessor macro encapsulating the header file prevents that `incr.h` is included multiple times.

`gcc` compiles both `incr.c` and `ctypes.c` and links both object files into an executable program  `ctypes`

```console
> gcc -ggdb -fomit-frame-pointer -o ctypes ctypes.c incr.c
```
If we execute  `ctypes` on the command line we get the results in *host order* i.e. as *little-endian* unsigned integer values packed into a byte array `buf`:
```console
> ./ctypes
buf = 0x7866554433221100bcaa9988deccef
```

**ASM 1**: <a name="asm1"></a>We load the  `ctypes` binary into the debugger, set breakpoints on lines `17`  and `19` of `main`, i.e. just before and after `incr()` is called and then run the program

```assembly
> gdb ctypes
(gdb) break 17
Breakpoint 1 at 0x771: file ctypes.c, line 17.
(gdb) break 19
Breakpoint 2 at 0x744: file ctypes.c, line 19.
(gdb) run
Starting program: /home/andi/cyber/Computing_Systems/ctypes
Breakpoint 1, main (argc=1, argv=0x7fffffffddc8) at ctypes.c:17
17	    incr(v64, v32, v16, v8, buf);
```
Now we disassemble `main` and show the instructions up to the return from the `incr()` function call
```assembly
(gdb) disassemble main
Dump of assembler code for function main:
   0x00005555555546da <+0>:   sub    $0x58,%rsp             ; lower %rsp by 0x58
   0x00005555555546de <+4>:   mov    %edi,0xc(%rsp)         ; copy argc to stack
   0x00005555555546e2 <+8>:   mov    %rsi,(%rsp)            ; copy argv to stack
   0x00005555555546e6 <+12>:  mov    %fs:0x28,%rax          ; init canary
   0x00005555555546ef <+21>:  mov    %rax,0x48(%rsp)        ; copy canary to stack
   0x00005555555546f4 <+26>:  xor    %eax,%eax              ; zero %eax
   0x00005555555546f6 <+28>:  movabs $0x11223344556677,%rax ; init v64 in %rax
   0x0000555555554700 <+38>:  mov    %rax,0x28(%rsp)        ; copy v64 to stack
   0x0000555555554705 <+43>:  movl   $0x8899aabb,0x20(%rsp) ; init v32 on stack
   0x000055555555470d <+51>:  movw   $0xccdd,0x1a(%rsp)     ; init v16 on stack
   0x0000555555554714 <+58>:  movb   $0xee,0x19(%rsp)       ; init v8  on stack
   0x0000555555554719 <+63>:  movl   $0xf,0x24(%rsp)        ; init len=15 on stack
=> 0x0000555555554721 <+71>:  movzbl 0x19(%rsp),%ecx        ; copy v8  to %ecx (arg4)
   0x0000555555554726 <+76>:  movzwl 0x1a(%rsp),%edx        ; copy v16 to %edx (arg3)
   0x000055555555472b <+81>:  lea    0x30(%rsp),%rdi        ; copy buf addr to %rdi
   0x0000555555554730 <+86>:  mov    0x20(%rsp),%esi        ; copy v32 to %esi (arg2)
   0x0000555555554734 <+90>:  mov    0x28(%rsp),%rax        ; copy v64 to %rax
   0x0000555555554739 <+95>:  mov    %rdi,%r8               ; copy buf addr to %r8 (arg5)
   0x000055555555473c <+98>:  mov    %rax,%rdi              ; copy v64 to %rdi (arg1)
   0x000055555555473f <+101>: callq  0x5555555547a3 <incr>  ; call incr
   0x0000555555554744 <+106>: lea    0x159(%rip),%rdi       # 0x5555555548a4
   ...
   End of assembler dump.
```
Since we are handling 8-, 16-, 32- and 64-bit unsigned integers but the registers have an intrinsic size of 64-bits the following suffixes are used for most instructions to restrict the size of the operands:

| Suffix | Type       | Operand Size  | Example |
|:------:|:-----------|:--------------|:--------|
|    b   | byte       |  8-bit integer | movb   |
|    w   | word       | 16-bit integer | movw   |
|    l   | doubleword | 32-bit integer | movl   |
|    q   | quadword   | 64-bit integer | movq   |

There are operations like `movz` that have two suffixes, e.g. `movzbl` or `movzwl` where the first suffix applies to the source and the second to the destination.

Next we check the current value of the stack pointer `%rsp` within `main` and determine the storage location of the local variables on the stack:

```assembly
(gdb) print/x $rsp
$1 = 0x7fffffffdc90
(gdb) print/x &argv
$2 = 0x7fffffffdc90
(gdb) print/x &argc
$3 = 0x7fffffffdc9c
(gdb) print/x &v8
$4 = 0x7fffffffdca9
(gdb) print/x &v16
$5 = 0x7fffffffdcaa
(gdb) print/x &v32
$6 = 0x7fffffffdcb0
(gdb) print/x &v64
$7 = 0x7fffffffdcb8
(gdb) print/x &buf
$8 = 0x7fffffffdcc0
(gdb) print/x argv
$9 = 0x7fffffffddc8
```
The last query above returns the content of the `argv` variable which contains a pointer to the `argv[]` array stored on the local stack of the shell that called the `main` program. All this collected address information will be later used to construct a linear map of the stack.

Since everything has been analyzed within the `main` program we now set a breakpoint just inside the `incr` function:
```assembly
(gdb) break incr
Breakpoint 3 at 0x5555555547a3: file incr.c, line 4.
(gdb) continue
Continuing.
Breakpoint 3, incr (v64=9, v32=0, v16=25927, v8=0 '\000', buf=0x0) at incr.c:4
4	{
```
The  `info frame` command gives a summary on the current stack and instruction pointer context:
```assembly
(gdb) info frame
Stack level 0, frame at 0x7fffffffdc90:
 rip = 0x5555555547a3 in incr (incr.c:4); saved rip = 0x555555554744
 called by frame at 0x7fffffffdcf0
 source language c.
 Arglist at 0x7fffffffdc80, args: v64=9, v32=0, v16=25927, v8=0 '\000', buf=0x0
 Locals at 0x7fffffffdc80, Previous frame's sp is 0x7fffffffdc90
 Saved registers:
  rip at 0x7fffffffdc88
```
We see that the instruction
```assembly
0x000055555555473f <+101>: callq  0x5555555547a3 <incr>  ; call incr
```
caused a jump from `0x55555555473f` in `main` to `0x5555555547a3` in `incr`.
The `info frame` information shows that the  `%rip` instruction pointer of `main` has been pushed to the stack at`0x7fffffffdc88` by the `callq` instruction so that the program flow can be resumed in `main` at the next instruction `0x555555554744` after the return from `incr`.

```assembly
(gdb) x/1xg 0x7fffffffdc88
0x7fffffffdc88:	0x0000555555554744
```
The assembly code of the function `incr` looks as follows:
```assembly
(gdb) disassemble incr
Dump of assembler code for function incr:
=> 0x00005555555547a3 <+0>:   mov    %rdi,-0x8(%rsp)  ; copy v64 to stack
   0x00005555555547a8 <+5>:   mov    %esi,-0xc(%rsp)  ; copy v32 to stack
   0x00005555555547ac <+9>:   mov    %ecx,%eax        ; copy v8 to %eax
   0x00005555555547ae <+11>:  mov    %r8,-0x20(%rsp)  ; copy buf addr to stack
   0x00005555555547b3 <+16>:  mov    %dx,-0x10(%rsp)  ; copy v16 to stack
   0x00005555555547b8 <+21>:  mov    %al,-0x14(%rsp)  ; copy v8 to stack
   0x00005555555547bc <+25>:  addq   $0x1,-0x8(%rsp)  ; add 1 to v64
   0x00005555555547c2 <+31>:  addl   $0x1,-0xc(%rsp)  ; add 1 to v32
   0x00005555555547c7 <+36>:  addw   $0x1,-0x10(%rsp) ; add 1 to v16
   0x00005555555547cd <+42>:  addb   $0x1,-0x14(%rsp) ; add 1 to v8
   0x00005555555547d2 <+47>:  mov    -0x20(%rsp),%rax ; copy buf addr to %rax
   0x00005555555547d7 <+52>:  mov    -0x8(%rsp),%rdx  ; copy v64 to %rdx
   0x00005555555547dc <+57>:  mov    %rdx,(%rax)      ; copy v64 to &buf[0]
   0x00005555555547df <+60>:  mov    -0x20(%rsp),%rax ; copy buf addr to %rax
   0x00005555555547e4 <+65>:  lea    0x8(%rax),%rdx   ; load buf addr+8 to %rdx
   0x00005555555547e8 <+69>:  mov    -0xc(%rsp),%eax  ; copy v32 to %eax
   0x00005555555547ec <+73>:  mov    %eax,(%rdx)      ; copy v32 to &buf[8]
   0x00005555555547ee <+75>:  mov    -0x20(%rsp),%rax ; copy buf addr to %rax
   0x00005555555547f3 <+80>:  lea    0xc(%rax),%rdx   ; load buf addr+12 to %rdx
   0x00005555555547f7 <+84>:  movzwl -0x10(%rsp),%eax ; copy v16 to %eax
   0x00005555555547fc <+89>:  mov    %ax,(%rdx)       ; copy v16 to &buf[12]
   0x00005555555547ff <+92>:  mov    -0x20(%rsp),%rax ; copy buf addr to %rax
   0x0000555555554804 <+97>:  lea    0xe(%rax),%rdx   ; load buf addr+14 to %rdx
   0x0000555555554808 <+101>: movzbl -0x14(%rsp),%eax ; copy v8 to %eax
   0x000055555555480d <+106>: mov    %al,(%rdx)       ; copy v8 to &buf[14]
   0x000055555555480f <+108>: nop                     ; no operation
   0x0000555555554810 <+109>: retq                    ; return
End of assembler dump.
```
Within the `incr` function we determine the value of the stack pointer `%rsp` and the storage location of all the local variables on the stack:
```assembly
(gdb) print/x $rsp
$10 = 0x7fffffffdc88
(gdb) print/x &v64
$11 = 0x7fffffffdc80
(gdb) print/x &v32
$12 = 0x7fffffffdc7c
(gdb) print/x &v16
$13 = 0x7fffffffdc78
(gdb) print/x &v8
$14 = 0x7fffffffdc74
(gdb) print/x &buf
$15 = 0x7fffffffdc68
```
From all the collected address information we can now construct a map of the processor stack:

| stack address    |%rsp offset| variable   | owner |
|:-----------------|:---------:|:-----------|:------|
| `0x7fffffffddc8` |           | argv[0]    | shell |
| `0x7fffffffdce8` |           | saved %rip | &#8592; %rsp of shell |
| `0x7fffffffdcd8` |   +0x48   | canary     | main  |
| `0x7fffffffdcc0` |   +0x30   | buf        | main  |
| `0x7fffffffdcb8` |   +0x28   | v64        | main  |
| `0x7fffffffdcb4` |   +0x24   | len        | main  |
| `0x7fffffffdcb0` |   +0x20   | v32        | main  |
| `0x7fffffffdcaa` |   +0x1a   | v16        | main  |
| `0x7fffffffdca9` |   +0x19   | v8         | main  |
| `0x7fffffffdc9c` |   +0x0c   | argc       | main  |
| `0x7fffffffdc90` |   +0x00   | argv       | &#8592; %rsp of main |
| `0x7fffffffdc88` |   -0x00   | saved %rip | &#8592; %rsp of incr |
| `0x7fffffffdc80` |   -0x08   | v64        | incr  |
| `0x7fffffffdc7c` |   -0x0c   | v32        | incr  |
| `0x7fffffffdc78` |   -0x10   | v16        | incr  |
| `0x7fffffffdc74` |   -0x14   | v8         | incr  |
| `0x7fffffffdc68` |   -0x20   |  buf addr  | incr  |

We now proceed to the next breakpoint which is located just after the return from function `incr'.
```assembly
(gdb) continue
Continuing.
Breakpoint 2, main (argc=1, argv=0x7fffffffddc8) at ctypes.c:19
19	    printf("buf = 0x");
gdb) x/i $rip
=> 0x555555554744 <main+106>: lea    0x159(%rip),%rdi        # 0x5555555548a4
```
We see that the instruction pointer of `main` has been retrieved from the stack by the `retq` instruction and that the execution resumes at `0x555555554744`.
```assembly
(gdb) x/15xb 0x7fffffffdcc0
0x7fffffffdcc0:	0x78	0x66	0x55	0x44	0x33	0x22	0x11	0x00
0x7fffffffdcc8:	0xbc	0xaa	0x99	0x88	0xde	0xcc	0xef
(gdb) continue
Continuing.
buf = 0x7866554433221100bcaa9988deccef
[Inferior 1 (process 26884) exited normally]
```
Before leaving the main program we dump the contents of the `buf` byte array containing the final result.

## Code Optimization <a name="section4"></a>

Now we are going to have a look at how object code is improved through applying compiler optimization using the `-O2` gcc option:
```console
> gcc -ggdb -fomit-frame-pointer -O2 -o ctypes ctypes.c incr.c
```
**ASM 2**: <a name="asm2"></a>Let's examine the optimized assembly code of `main` and `incr`
```assembly
(gdb) disassemble main
Dump of assembler code for function main:
   0x00005555555545f0 <+0>:  push   %r12                   ; save %r12 on stack
   0x00005555555545f2 <+2>:  push   %rbp                   ; save %rbp on stack
=> 0x00005555555545f3 <+3>:  mov    $0xee,%ecx             ; init v8  in %ecx (arg4)
   0x00005555555545f8 <+8>:  push   %rbx                   ; save %rbx on stack
   0x00005555555545f9 <+9>:  mov    $0xccdd,%edx           ; init v16 in %edx (arg3)
   0x00005555555545fe <+14>: mov    $0x8899aabb,%esi       ; init v32 in %esi (arg2)
   0x0000555555554603 <+19>: movabs $0x11223344556677,%rdi ; init v64 in %rdi (arg1)
   0x000055555555460d <+29>: lea    0x229(%rip),%rbp       # 0x55555555483d
   0x0000555555554614 <+36>: sub    $0x20,%rsp             ; lower %rsp by 0x20
   0x0000555555554618 <+40>: mov    %rsp,%rbx              ; copy buf addr to %rbx
   0x000055555555461b <+43>: mov    %rbx,%r8               ; copy buf addr to %r8 (arg5)
   0x000055555555461e <+46>: lea    0xf(%rbx),%r12         ; copy addr above buf to %r12
   0x0000555555554622 <+50>: mov    %fs:0x28,%rax          ; init canary
   0x000055555555462b <+59>: mov    %rax,0x18(%rsp)        ; copy canary to stack
   0x0000555555554630 <+64>: xor    %eax,%eax              ; zero %eax
   0x0000555555554632 <+66>: callq  0x555555554790 <incr>  ; call incr
   0x0000555555554637 <+71>: lea    0x1f6(%rip),%rsi       # 0x555555554834
   ...
   End of assembler dump
(gdb) disassemble incr
Dump of assembler code for function incr:
=> 0x0000555555554790 <+0>:  add    $0x1,%rdi              ; add 1 to v64
   0x0000555555554794 <+4>:  add    $0x1,%esi4             ; add 1 to v32
   0x0000555555554797 <+7>:  add    $0x1,%edx              ; add 1 to v16
   0x000055555555479a <+10>: add    $0x1,%ecx              ; add 1 to v8
   0x000055555555479d <+13>: mov    %rdi,(%r8)             ; copy v64 to %buf[ 0]
   0x00005555555547a0 <+16>: mov    %esi,0x8(%r8)          ; copy v32 to %buf[ 8]
   0x00005555555547a4 <+20>: mov    %dx,0xc(%r8)           ; copy v16 to %buf[12]
   0x00005555555547a9 <+25>: mov    %cl,0xe(%r8)           ; copy v8  to %buf[14]
   0x00005555555547ad <+29>: retq                          ; return
   End of assembler dump
```
With the exception of the large `buf` byte array all variables are strictly kept in fast processor registers. This is also visible in the stack memory map below:

|  stack address   |%rsp offset|variable    | owner |
|:----------------:|:---------:|:-----------|:------|
| `0x7fffffffdd18` |           | saved %rip | &#8592; %rsp of shell |
| `0x7fffffffdd10` |           | saved %r12 | main  |
| `0x7fffffffdd08` |           | saved %rbp | main  |
| `0x7fffffffdd00` |           | saved %rbx | main  |
| `0x7fffffffdcf8` |   +0x18   | canary     | main  |
| `0x7fffffffdce0` |   +0x00   | buf        | &#8592; %rsp of main |
| `0x7fffffffdcd8` |   -0x00   | saved %rip | &#8592; %rsp of incr |

Actually the reason that the `incr()`  function was moved to a separate file `incr.c` was the fact that the optimizing compiler would just have got rid of the function call and implemented the functionality directly in `main` as in-line code.

## The Heap <a name="section5"></a>
We slightly modify the `ctypes` program in order to show the use of dynamically generated data objects that are stored on the *heap*.

**C 3**: <a name="c3"></a> The C file [dyn_incr.c](dyn_incr.c) implements a function `incr()` which increments its unsigned integer arguments `v64`, `v32`,  `v16` and `v8` by  an offset `n` and returns the results in *network order*  in a dynamically allocated byte arrary :

```C
 1 #include <stdlib.h>
 2 #include <arpa/inet.h>
 3
 4 #include "dyn_incr.h"
 5
 6 uint8_t* incr(uint64_t v64, uint32_t v32, uint16_t v16, uint8_t v8, uint8_t n)
 7 {
 8     uint8_t *buf = (uint8_t*)malloc(16);
 9
10     /* increment inputs by n */
11     v64 += n;
12     v32 += n;
13     v16 += n;
14     v8  += n;
15
16     /* cast results in network order into buffer */
17     *((uint32_t*)&buf[0])  = htonl(v64 >> 32);
18     *((uint32_t*)&buf[4])  = htonl(v64 & 0x0000000ffffffff);
19     *((uint32_t*)&buf[8])  = htonl(v32);
20     *((uint16_t*)&buf[12]) = htons(v16);
21     *((uint8_t*) &buf[14]) = v8;
22
23     return buf;
24 }
```
This change requires an update of the `incr()` interface definition in the [dyn_incr.h](dyn_incr.h) header file
```C
 1 #ifndef DYN_INCR_H_
 2 #define DYN_INCR_H_

#include <stdint.h>

1 /**
2  * Increments each of the inputs v64, v32, v16, and v8 by n and returns
3  * the results in a network order packed byte array
4  */
5 uint8_t* incr(uint64_t v64, uint32_t v32, uint16_t v16, uint8_t v8, uint8_t n);
6
7 #endif /** DYN_INCR_H_ */
```
In the main program [dyn_ctypes.c](dyn_ctypes.c) we call the function `incr()` twice with different increments and print the resulting outputs.
```C
 1 #include <stdlib.h>
 2 #include <stdio.h>
 3
 4 #include "dyn_incr.h"
 5
 6 /* global uninitialized variables */
 7 uint8_t buf[16]
 8    
 9 /* global initialized variables */
10 uint64_t v64 = 0x0011223344556677;
11 uint32_t v32 = 0x8899aabb;
12
13 /* global constants */
14 const uint16_t v16 = 0xccdd;
15 const uint8_t  v8  = 0xee;
16
17 int main(int argc, char** argv)
18 {
19     uint8_t *buf1, *buf2;
20     const int len = sizeof(uint64_t) + sizeof(uint32_t) +
21                     sizeof(uint16_t) + sizeof(uint8_t);
22     int i;
23
24     buf1 = incr(v64, v32, v16, v8, 1);
25     buf2 = incr(v64, v32, v16, v8, 2);
26
27     printf("buf1 = 0x");
28     for (i = 0; i < len; i++)
29     {
30         printf("%02x", buf1[i]);
31     }
32     printf("\n");
33
34     printf("buf2 = 0x");
35     for (i = 0; i < len; i++)
36     {
37         printf("%02x", buf2[i]);
38     }
39     printf("\n");
40
41     free(buf1);
42     free(buf2);
43     exit(0);
44 }
```
We compile the program with optimization level `-O2` and execute `dyn_ctypes`
```console
> gcc -ggdb -fomit-frame-pointer -O2 -o dyn_ctypes dyn_ctypes.c dyn_incr.c
> ./dyn_ctypes
buf1 = 0x00112233445566788899aabcccdeef
buf2 = 0x00112233445566798899aabdccdff0
```
As you can clearly see the integer variables packed into the buffer are now in *network order* with most significant byte stored first..

**ASM 3**: <a name="asm3"></a>We analyze `dyn_ctypes`  by setting  a breakpoint at line `23` of `dyn_incr.c`, i.e. just before returning to the main program.

```assembly
> gdb dyn_ctypes
(gdb) break dyn_incr.c:23
Breakpoint 1 at 0x8f2: file dyn_incr.c, line 23.
(gdb) run
Starting program: /home/andi/cyber/Computing_Systems/dyn_ctypes
Breakpoint 1, incr (v64=4822678189205112, v32=<optimized out>, v16=52446, v8=<optimized out>, n=<optimized out>) at dyn_incr.c:24
24	}
```
The optimized machine code uses a lot of registers so that some of them must be saved to the stack first. We also notice with surprise that the call of the  `htonl()` library function has been replaced by the `bswap` instruction and `htons()` by a `ror 0x8` instruction which rotates a 16-bit word by 8 bits thus swapping the order of the two bytes.
```assembly
Dump of assembler code for function incr:
   0x0000555555554890 <+0>:   push   %r15               ; save %r15 on stack
   0x0000555555554892 <+2>:   push   %r14               ; save %r14 on stack
   0x0000555555554894 <+4>:   mov    %r8d,%r15d         ; copy n to %r15d
   0x0000555555554897 <+7>:   push   %r13               ; save %r13 on stack
   0x0000555555554899 <+9>:   push   %r12               ; save %r12 on stack
   0x000055555555489b <+11>:  mov    %rdi,%r14          ; copy v64 to %r14
   0x000055555555489e <+14>:  push   %rbp               ; save %rbp on stack
   0x000055555555489f <+15>:  push   %rbx               ; save %rbx on stack
   0x00005555555548a0 <+16>:  mov    $0x10,%edi         ; init %edi (arg1) wit 16 (malloc)
   0x00005555555548a5 <+21>:  mov    %edx,%r12d         ; copy v16 to %r12d
   0x00005555555548a8 <+24>:  mov    %esi,%r13d         ; copy v32 to %r13d
   0x00005555555548ab <+27>:  mov    %ecx,%ebp          ; copy v8 to %ebp
   0x00005555555548ad <+29>:  sub    $0x8,%rsp          ; lower %rsp by 0x08
   0x00005555555548b1 <+33>:  add    %r15d,%ebp         ; add n to v8
   0x00005555555548b4 <+36>:  callq  0x555555554640 <malloc@plt> ; alloc buf (%rax)
   0x00005555555548b9 <+41>:  movzbl %r15b,%edi         ; copy n to %edi
   0x00005555555548bd <+45>:  movzbl %r15b,%esi         ; copy n to %esi
   0x00005555555548c1 <+49>:  mov    %bpl,0xe(%rax)     ; copy v8 to &buf[14]
   0x00005555555548c5 <+53>:  add    %r14,%rdi          ; add n to v64
   0x00005555555548c8 <+56>:  add    %esi,%r13d         ; add n to v32
   0x00005555555548cb <+59>:  movzbl %r15b,%esi         ; copy n to %esi
   0x00005555555548cf <+63>:  mov    %rdi,%rdx          ; copy v64 to %rdx
   0x00005555555548d2 <+66>:  bswap  %r13d              ; swap byte order of v32
   0x00005555555548d5 <+69>:  bswap  %edi               ; swap byte order of v64-low
   0x00005555555548d7 <+71>:  shr    $0x20,%rdx         ; v64 >> 32
   0x00005555555548db <+75>:  mov    %edi,0x4(%rax)     ; copy v64-low to &buf[4]
   0x00005555555548de <+78>:  mov    %r13d,0x8(%rax)    ; copy v32 to %buf[8]
   0x00005555555548e2 <+82>:  bswap  %edx               ; swap byte order of v64-hi
   0x00005555555548e4 <+84>:  mov    %edx,(%rax)        ; copy v64-hi to &buf[0]
   0x00005555555548e6 <+86>:  lea    (%rsi,%r12,1),%edx ; add n to v16
   0x00005555555548ea <+90>:  ror    $0x8,%dx           ; rotate v16 by 8 bits
   0x00005555555548ee <+94>:  mov    %dx,0xc(%rax)      ; copy v16 to &buf[12]
=> 0x00005555555548f2 <+98>:  add    $0x8,%rsp          ; increase %rsp by 0x08
   0x00005555555548f6 <+102>: pop    %rbx               ; restore %rbx
   0x00005555555548f7 <+103>: pop    %rbp               ; restore %rbp
   0x00005555555548f8 <+104>: pop    %r12               ; restore %r12
   0x00005555555548fa <+106>: pop    %r13               ; restore %r13
   0x00005555555548fc <+108>: pop    %r14               ; restore %r14
   0x00005555555548fe <+110>: pop    %r15               ; restore %r15
   0x0000555555554900 <+112>: retq                      ; return
End of assembler dump.
```
The `info frame` shows that besides the  `%rip` instruction pointer all six [callee-saved registers](#section2) have been pushed on the stack.
```assembly
(gdb) info frame
Stack level 0, frame at 0x7fffffffdcd0:
 rip = 0x5555555548f2 in incr (dyn_incr.c:24); saved rip = 0x5555555546b1
 called by frame at 0x7fffffffdd00
 source language c.
 Arglist at 0x7fffffffdc88, args: v64=4822678189205112, v32=<optimized out>, v16=52446, v8=<optimized out>, n=<optimized out>
 Locals at 0x7fffffffdc88, Previous frame's sp is 0x7fffffffdcd0
 Saved registers:
  rbx at 0x7fffffffdc98, rbp at 0x7fffffffdca0, r12 at 0x7fffffffdca8,
  r13 at 0x7fffffffdcb0, r14 at 0x7fffffffdcb8, r15 at 0x7fffffffdcc0,
  rip at 0x7fffffffdcc8
```
We check the heap address of the byte buffer returned by `incr()`
```assembly
(gdb) print/z &buf
Address requested for identifier "buf" which is in register $rax
(gdb) print/z buf
$1 = 0x0000555555756260
```
and then proceed to the second call of `incr()`
```assembly
(gdb) continue
Continuing.
Breakpoint 1, incr (v64=4822678189205113, v32=<optimized out>, v16=52447, v8=<optimized out>, n=<optimized out>) at dyn_incr.c:24
24	}
(gdb) print/z &buf
Address requested for identifier "buf" which is in register $rax
(gdb) print/z buf
$2 = 0x0000555555756280
```
Then we return to the main program
```assembly
(gdb) break dyn_ctypes.c:27
Breakpoint 2 at 0x55555555470e: file dyn_ctypes.c, line 27.
(gdb) continue
Continuing.
Breakpoint 2, main (argc=<optimized out>, argv=<optimized out>) at dyn_ctypes.c:28
28	    for (i = 0; i < len; i++)
```
Again we notice that the optimized code needs a lot of registers
```assembly
 (gdb) disassemble main
Dump of assembler code for function main:
   0x0000555555554680 <+0>:   mov    0x20098a(%rip),%esi   # 0x555555755010 <v32> (arg2)
   0x0000555555554686 <+6>:   mov    0x20098b(%rip),%rdi   # 0x555555755018 <v64> (arg1)
   0x000055555555468d <+13>:  mov    $0x1,%r8d             ; set n to 1 in %r8d (arg5)
   0x0000555555554693 <+19>:  push   %r14                  ; save %r14
   0x0000555555554695 <+21>:  push   %r13                  ; save %r13
   0x0000555555554697 <+23>:  mov    $0xee,%ecx            ; init v8 in %ecx (arg4)
   0x000055555555469c <+28>:  push   %r12                  ; save %r12
   0x000055555555469e <+30>:  push   %rbp                  ; save %rpb
   0x000055555555469f <+31>:  mov    $0xccdd,%edx          ; init v16 in %edx (arg3)
   0x00005555555546a4 <+36>:  push   %rbx                  ; save %rbx
   0x00005555555546a5 <+37>:  lea    0x2f2(%rip),%r13      # 0x55555555499e
   0x00005555555546ac <+44>:  callq  0x555555554890 <incr> ; call incr
   0x00005555555546b1 <+49>:  mov    0x200959(%rip),%esi   # 0x555555755010 <v32> (arg2)
   0x00005555555546b7 <+55>:  mov    0x20095a(%rip),%rdi   # 0x555555755018 <v64> (arg1)
   0x00005555555546be <+62>:  mov    %rax,%r12             ; copy buf1 addr to %r12
   0x00005555555546c1 <+65>:  mov    $0x2,%r8d             ; set n to 2 in %r8d (arg5)
   0x00005555555546c7 <+71>:  mov    $0xee,%ecx            ; init v8 in %ecx (arg4)
   0x00005555555546cc <+76>:  mov    $0xccdd,%edx          ; init v16 in %edx (arg3)
   0x00005555555546d1 <+81>:  lea    0xf(%r12),%r14        ; copy addr above buf1 to %r14
   0x00005555555546d6 <+86>:  mov    %r12,%rbx             ; copy buf1 addr to %rbx
   0x00005555555546d9 <+89>:  callq  0x555555554890 <incr> ; call incr
   0x00005555555546de <+94>:  lea    0x2af(%rip),%rsi      # 0x555555554994
   0x00005555555546e5 <+101>: mov    %rax,%rbp             ; copy buf2 addr to %rbp
   ...
   End of assembler dump.
```
The main program pushes the `%rip` and five [callee-saved registers](#section2) on the stack
```assembly
(gdb) info frame
Stack level 0, frame at 0x7fffffffdd00:
 rip = 0x55555555470e in main (dyn_ctypes.c:28); saved rip = 0x7ffff7a05b97
 source language c.
 Arglist at 0x7fffffffdcc8, args: argc=<optimized out>, argv=<optimized out>
 Locals at 0x7fffffffdcc8, Previous frame's sp is 0x7fffffffdd00
 Saved registers:
  rbx at 0x7fffffffdcd0, rbp at 0x7fffffffdcd8, r12 at 0x7fffffffdce0,
  r13 at 0x7fffffffdce8, r14 at 0x7fffffffdcf0, rip at 0x7fffffffdcf8
```
As an additional source code change we have defined `v64` and `v32` as *global initialized variables* and `v16` and `v8` as *global constants*. We also added an unused *global initialized* byte array `buf[16]` that is automatically initialized to all zeros when the C program starts up. As you can see all these global [or static] variables and constants do not reside on the stack but are loaded just above the program memory.
```assembly
(gdb) print/x &v64
$3 = 0x555555755018
(gdb) print/x &v32
$4 = 0x555555755010
(gdb) print/x &v16
$5 = 0x5555555549b0
(gdb) print/x &v8
$6 = 0x5555555549ae
(gdb) print/x &buf
$7 = 0x555555755040
(gdb) x/16b buf
0x555555755040 <buf>:	0x00	0x00	0x00	0x00	0x00	0x00	0x00	0x00
0x555555755048 <buf+8>:	0x00	0x00	0x00	0x00	0x00	0x00	0x00	0x00
```
We can also verify that addresses of the dynamically allocated byte arrays `buf1` and `buf2` have been made available in the main program.l
```assembly
(gdb) print/x &buf1
Address requested for identifier "buf1" which is in register $r12
(gdb) print/x buf1
$8 = 0x555555756260
(gdb) x/15b 0x555555756260
0x555555756260:	0x00	0x11	0x22	0x33	0x44	0x55	0x66	0x78
0x555555756268:	0x88	0x99	0xaa	0xbc	0xcc	0xde	0xef
(gdb) print/x &buf2
Address requested for identifier "buf2" which is in register $rbp
(gdb) print/x buf2
$9 = 0x555555756280
(gdb) x/15xb 0x555555756280
0x555555756280:	0x00	0x11	0x22	0x33	0x44	0x55	0x66	0x79
0x555555756288:	0x88	0x99	0xaa	0xbd	0xcc	0xdf	0xf0
```
And as the last component in our memory puzzle the command `info sharedlibrary`  shows the location of the *shared libraries* needed to run the `dyn_ctypes` program:
```assembly
(gdb) info sharedlibrary
From                To                  Shared Object Library
0x00007ffff7dd5f10  0x00007ffff7df4b20  /lib64/ld-linux-x86-64.so.2
0x00007ffff7a052d0  0x00007ffff7b7dc3c  /lib/x86_64-linux-gnu/libc.so.6
```
Upon leaving the main program these two buffers are printed

```console
(gdb) continue
Continuing.
buf1 = 0x00112233445566788899aabcccdeef
buf2 = 0x00112233445566798899aabdccdff0
[Inferior 1 (process 20469) exited normally]
```
Based on all the collected address information we can now draw the following virtual memory map of an x86-64 process:

| memory address   | content    | type                 |
|:----------------:|:-----------|:---------------------|
| `0x7fffffffdcf8` | saved %rip | stack main           |
| `0x7fffffffdcf0` | saved %r14 | stack main           |
| `0x7fffffffdce8` | saved %r13 | stack main           |
| `0x7fffffffdce0` | saved %r12 | stack main           |
| `0x7fffffffdcd8` | saved %rbp | stack main           |
| `0x7fffffffdcd0` | saved %rbx | &#8592; %rsp of main |
| `0x7fffffffdcc8` | saved %rip | stack incr           |
| `0x7fffffffdcc0` | saved %r15 | stack incr           |
| `0x7fffffffdcb8` | saved %r14 | stack incr           |
| `0x7fffffffdcb0` | saved %r13 | stack incr           |
| `0x7fffffffdca8` | saved %r12 | stack incr           |
| `0x7fffffffdca0` | saved %rbp | stack incr           |
| `0x7fffffffdc98` | saved %rbx | &#8592; %rsp of incr |
|      &#8595;     |            | stack grows downwards|
|      &nbsp;      |            |                      |
| `0x7ffff7df4b20` |            | shared libraries   |
| `0x7ffff7dd5f10` | /lib64/ld-linux-x86-64.so.2| shared libraries |
| `0x7ffff7b7dc3c` |            | shared libraries  |
| `0x7ffff7a052d0` | /lib/x86_64-linux-gnu/libc.so.6| shared libraries |
|      &nbsp;      |            |                      |
|      &#8593;     |            | heap grows upwards   |
| `0x555555756280` | buf2       | heap                 |
| `0x555555756260` | buf1       | heap                 |
| `0x555555755040` | buf        | global/static uninitialized variables |
| `0x555555755018` | v64        | global/static initialized variables  |
| `0x555555755010` | v32        | global/static initialized variables  |
| `0x5555555549b0` | v16        | global/static constants              |
| `0x5555555549ae` | v8         | global/static constants              |
| `0x555555554900` |            | text incr            |
| `0x555555554890` | incr       | text incr            |
| `0x555555554777` |            | text main            |
| `0x555555554680` | main       | text main            |

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
