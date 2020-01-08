# Programs

## Table of Contents
1. [Simple Example Algorithm](#section1)

C language programs: &nbsp;[C1](#c1)

Assembler exercises: &nbsp; [ASM1](#asm1) &nbsp; [ASM2](#asm2)

## Simple Example Algorithm <a name="section1"></a>

Our simple example algorithm computes  the sum of the first `n` natural numbers in a `while` loop.

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
 9     while (n)
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
We compile the C program with the `gcc` command
```console
> gcc -fomit-frame-pointer -o sum sum.c
```
and execute the binary program `sum` on the command line
```console
> ./sum 5
n = 5, s = 15
```
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
   0x00000000004005d1 <+27>: cmpl   $0x0,-0x14(%rsp)    ; compare if n == 0
   0x00000000004005d6 <+32>: jne    0x4005c4 <sum+14>   ; jump to head of loop if n != 0
   0x00000000004005d8 <+34>: mov    -0x4(%rsp),%eax     ; copy result s into register
   0x00000000004005dc <+38>: retq                       ; return to main program
End of assembler dump.
```

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/