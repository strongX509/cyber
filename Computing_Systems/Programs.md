# _Programs

## Table of Contents
1. [From Algorithm to Program](#section1)
2. [Disassembling Object Code](#section2)
3. [x86-64 Processor Architecture](#section3)
4. [Processor Stack](#section4)
4. [List of gdb Shell Commands](#section5)

C language programs: &nbsp;[C1](#c1) &nbsp;  [C2](#c2)

Assembler exercises: &nbsp; [ASM1](#asm1)  &nbsp; [ASM2](#asm2)

## From Algorithm to Program <a name="section1"></a>

The following simple algorithm defined in *pseudo code*  computes and returns the sum of the first `n` natural numbers:

---
**function** *sum(n)*  
 &nbsp; s &#8592; 0  
 &nbsp; while n > 0  
 &nbsp; &nbsp;  s &#8592; s + n  
 &nbsp; &nbsp;  n &#8592; n - 1  
 &nbsp; return s

---
This algorithm can be easily implemented in the C programming language.

**C 1**: <a name="c1"></a> The C program [sum.c](sum.c) consists of a function `sum()`and a main program `main()`and is listed with line numbers below:

```C
 1 #include <stdlib.h>
 2 #include <stdio.h>
 3
 4 /* computes the sum of the first n natural numbers */
 5 int sum(int n)
 6 {
 7     int s = 0;
 8
 9     while (n > 0)
10     {
11         s += n;
12         n -= 1;
13     }
14     return s;
15 }
16
17 /* main program */
18 int main(int argc, char** argv)
19 {
20     int n;
21
22    /* we expect at least one command line argument */
23    if (argc < 2)
24     {
25         exit(1);
26     }
27     n = atoi(argv[1]);
28
29     printf("n = %d, s = %d\n", n, sum(n));
30     exit(0);
31 }
```
We compile the C program with the `gcc` command where the `-ggdb` option adds debug information to the object code.
```console
> gcc -ggdb -fomit-frame-pointer -o sum sum.c
```
and execute the binary program `sum` on the command line
```console
> ./sum 3
n = 3, s = 6
```

## Disassembling Object Code <a name="section2"></a>

The `gdb` debugger is a very powerful command line tool for the dynamic analysis and debugging of object code.

**ASM 1**: <a name="asm1"></a>With the command 

```console
> gdb sum
```
we load the generated object code `sum` into the debugger and disassemble the machine instructions for the function `sum`
```assembly
(gdb) disassemble sum
Dump of assembler code for function sum:
   0x00000000004005b6 <+0> : mov    %edi,-0x14(%rsp)    ; copy argument n to stack
   0x00000000004005ba <+4> : movl   $0x0,-0x4(%rsp)     ; initialize sum s on stack
   0x00000000004005c2 <+12>: jmp    0x4005d1 <sum+27>   ; jump to the end of the loop
   0x00000000004005c4 <+14>: mov    -0x14(%rsp),%eax    ; copy current n into register
   0x00000000004005c8 <+18>: add    %eax,-0x4(%rsp)     ; add n to s
   0x00000000004005cc <+22>: subl   $0x1,-0x14(%rsp)    ; decrease n by one
   0x00000000004005d1 <+27>: cmpl   $0x0,-0x14(%rsp)    ; compare n with 0
   0x00000000004005d6 <+32>: jg     0x4005c4 <sum+14>   ; jump to head of loop if n > 0
   0x00000000004005d8 <+34>: mov    -0x4(%rsp),%eax     ; copy result s into register
   0x00000000004005dc <+38>: retq                       ; return to main program
End of assembler dump.
```
We set a *breakpoint* at line `11` of the C source code which is just before the sum is updated with the statement `s += n`. Then we start the execution with `run 3` setting the input argument to `n = 3`.
```assembly
(gdb) break 11
Breakpoint 1 at 0x4005c4: file sum.c, line 11.
(gdb) run 3
Starting program: /home/andi/cyber/Computing_Systems/sum 3

Breakpoint 1, sum (n=3) at sum.c:11
11	        s += n;
```
The 64 bit `rip` *instruction pointer register* points to the next machine instruction to be executed and can be displayed with the following command
```assembly
(gdb) x/i $rip
=> 0x4005c4 <sum+14>:	mov    -0x14(%rsp),%eax
```
We can print out the current state of the sum variable `s` and then `continue` the program execution until the *breakpoint* is hit again in the next iteration of the loop
```assembly
(gdb) print s
$1 = 0
(gdb) continue
Continuing.

Breakpoint 1, sum (n=2) at sum.c:11
11	        s += n;
(gdb) print s
$2 = 3
(gdb) continue
Continuing.

Breakpoint 1, sum (n=1) at sum.c:11
11	        s += n;
(gdb) print s
$3 = 5
```
With `n = 1` we are now in the last iteration and `step` through the program execution *line by line* until we reach the exit from the `while` loop. If a program line comprises more than one machine instruction then `stepi` allows to step through them.
```assembly
(gdb) step
12	        n -= 1;
(gdb) x/i $rip
=> 0x4005cc <sum+22>:	subl   $0x1,-0x14(%rsp)
(gdb) print s
$4 = 6
(gdb) step
9	    while (n > 0)
(gdb) x/i $rip
=> 0x4005d1 <sum+27>:	cmpl   $0x0,-0x14(%rsp)
(gdb) print n
$5 = 0
(gdb) stepi
0x00000000004005d6	9	    while (n > 0)
(gdb) x/i $rip
=> 0x4005d6 <sum+32>:	jg     0x4005c4 <sum+14>
(gdb) step
14	    return s;
(gdb) x/i $rip
=> 0x4005d8 <sum+34>:	mov    -0x4(%rsp),%eax
(gdb) continue
Continuing.
```
After the return from the `sum` function  the `main` program resumes its execution and prints out the final result
```assembly
n = 3, s = 6
[Inferior 1 (process 27469) exited normally]
```
We exit the debugger with `quit`.

##  x86-64 Processor Architecture <a name="section3"></a>

The general architecture of a Central Processing Unit (CPU) is shown in the diagram below:
![Processor Architecture][PROC_ARCH_IMG]

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

In the [assembly code](#asm1) of the function `sum` listed in the [previous section](#asm1) we see that the 32-bit `%edi` register is used for the single input parameter `n`  of type `int` and the 32-bit `%eax` register returns the `int` value of `s` back to the main program. Also the current value of the stack pointer `%rsp` is used to reference memory locations on the stack.

[PROC_ARCH_IMG]: Processor_600.png "Courtesy of Phil Janson"
[X86_CS]: https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf

## Processor Stack <a name="section4"></a>

**C 2**: <a name="c2"></a> The C file [incr.c](incr.c) implements a function `incr()` which increments its unsigned integer arguments `v64`, `v32`,  `v16` and `v8` by one and returns the results in *host order* packed into a byte arrary `buf`:

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

**ASM 2**: <a name="asm2"></a>Now we load `ctypes` into the debugger, set breakpoints on lines `17`  and `19`, i.e. just before and after `incr()` is called and then run the program

```assembly
> gdb sum
(gdb) break 17
Breakpoint 1 at 0x771: file ctypes.c, line 17.
(gdb) break 19
Breakpoint 2 at 0x744: file ctypes.c, line 19.
(gdb) run
Starting program: /home/andi/cyber/Computing_Systems/ctypes

Breakpoint 1, main (argc=1, argv=0x7fffffffddc8) at ctypes.c:17
17	    incr(v64, v32, v16, v8, buf);
```
Now we disassemble `main` and show the instructions up to the `incr()` function call
```assembly
(gdb) disassemble main
Dump of assembler code for function main:
   0x00005555555546da <+0>:   sub    $0x58,%rsp             ; local stack size 0x58
   0x00005555555546de <+4>:   mov    %edi,0xc(%rsp)         ; copy argc to stack
   0x00005555555546e2 <+8>:   mov    %rsi,(%rsp)            ; copy argv to stack
   0x00005555555546e6 <+12>:  mov    %fs:0x28,%rax          ; initialize canary
   0x00005555555546ef <+21>:  mov    %rax,0x48(%rsp)        ; copy canary to stack
   0x00005555555546f4 <+26>:  xor    %eax,%eax              ; zero %eax
   0x00005555555546f6 <+28>:  movabs $0x11223344556677,%rax ; initialize v64 in %rax
   0x0000555555554700 <+38>:  mov    %rax,0x28(%rsp)        ; copy v64 to stack
   0x0000555555554705 <+43>:  movl   $0x8899aabb,0x20(%rsp) ; initialize v32 on stack
   0x000055555555470d <+51>:  movw   $0xccdd,0x1a(%rsp)     ; initialize v16 on stack
   0x0000555555554714 <+58>:  movb   $0xee,0x19(%rsp)       ; initialize v8  on stack
   0x0000555555554719 <+63>:  movl   $0xf,0x24(%rsp)        ; initialize len=15 on stack
=> 0x0000555555554721 <+71>:  movzbl 0x19(%rsp),%ecx        ; copy v8  to %ecx (arg 4)
   0x0000555555554726 <+76>:  movzwl 0x1a(%rsp),%edx        ; copy v16 to %edx (arg 3)
   0x000055555555472b <+81>:  lea    0x30(%rsp),%rdi        ; copy buf addr to %rdi
   0x0000555555554730 <+86>:  mov    0x20(%rsp),%esi        ; copy v32 to %esi (arg 2)
   0x0000555555554734 <+90>:  mov    0x28(%rsp),%rax        ; copy v64 to %rax
   0x0000555555554739 <+95>:  mov    %rdi,%r8               ; copy buf addr to %r8 (arg 5)
   0x000055555555473c <+98>:  mov    %rax,%rdi              ; copy v64 to %rdi (arg 1)
   0x000055555555473f <+101>: callq  0x5555555547a3 <incr>  ; call incr
   0x0000555555554744 <+106>: lea    0x159(%rip),%rdi       # 0x5555555548a4
   ...
   End of assembler dump.
```
We check the current value of the stack pointer `%rsp` within `main` and determine the storage location of the local variables on the stack:
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
```assembly
(gdb) break incr:1
Breakpoint 3 at 0x5555555547a3: file incr.c, line 4.
(gdb) continue
Continuing.

Breakpoint 3, incr (v64=9, v32=0, v16=25927, v8=0 '\000', buf=0x0) at incr.c:4
4	{
```
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
(gdb) x/8b 0x7fffffffdc88
0x7fffffffdc88:	0x44 0x47 0x55 0x55 0x55 0x55 0x00 0x00
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

```assembly
(gdb) continue
Continuing.

Breakpoint 2, main (argc=1, argv=0x7fffffffddc8) at ctypes.c:19
19	    printf("buf = 0x");
gdb) x/i $rip
=> 0x555555554744 <main+106>: lea    0x159(%rip),%rdi        # 0x5555555548a4
(gdb) x/15b 0x7fffffffdcc0
0x7fffffffdcc0:	0x78	0x66	0x55	0x44	0x33	0x22	0x11	0x00
0x7fffffffdcc8:	0xbc	0xaa	0x99	0x88	0xde	0xcc	0xef
(gdb) continue
Continuing.
buf = 0x7866554433221100bcaa9988deccef
[Inferior 1 (process 26884) exited normally]
```

##  List of gdb Shell Commands <a name="section5"></a>

| Command        | Action                                          |
|:---------------|:------------------------------------------------|
| help [command] | Help information on a command                   |
| break `11`     | Set a breakpoint on line `11`                   |
| run [options]  | Start program execution with optional arguments |
| continue       | Continue program execution                      |
| step           | Execute next program line                       |
| stepi          | Execute next machine instruction                |
| print `n`      | Value of variable `n`                           |
| print `&n`     | Memory address of variable `n`                  |
| print/x `$rax` | Value of register `%rax` in hex format          |
| x/i `$rip`     | Value of instruction pointer and next instruction |
| info frame     | Information on instruction pointer and stack     |
| x/8xb `0x7fffffffa0b6` | Shows 8 bytes starting from `0x7fffffffa0b6` |
| quit           | Exit gdb                                         |

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
