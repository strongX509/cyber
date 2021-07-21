# Programs

## Table of Contents
1. [From Algorithm to Program](#section1)
2. [Disassembling Object Code](#section2)
3. [List of gdb Shell Commands](#section3)

C language programs: &nbsp;[C1](#c1)

Assembler exercises: &nbsp; [ASM1](#asm1)

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

**ASM 1**: <a name="asm1"></a> With the following command the generated object code `sum` is loaded into the debugger

```console
> gdb sum
```
With the `-n` option, commands from any `.gdbinit` initialization files can be prevented from executing, like e.g. the [Python Exploit Development Assistance (PEDA)][PEDA] script that we are not using in this tutorial.

The `gdb`debugger has two styles with which to display assembly code: `intel` for Intel style that is popular mostly among *Windows* users and `att` for AT&T style that is popular with *Linux* users. The following command shows the default style which depends on the Linux distribution:

```assembly
(gdb) show disassembly-flavor
The disassembly flavor is "intel".
```
In all our tutorials we are going to work with the `att` style. So if the default is set to `intel`, just switch the disassembly style with
```assembly
(gdb) set disassembly-flavor att
(gdb) show disassembly-flavor
The disassembly flavor is "att".
```
Now we are ready to disassemble the machine instructions for the function `sum`
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
In the assembly code of the function `sum` we see that the 32-bit `%edi` register is used to transfer the single input parameter `n`  of type `int` and the 32-bit `%eax` register returns the `int` value of `s` back to the main program. Also the current value of the stack pointer `%rsp` is used to reference memory locations on the stack. The details on the use of a processor's registers and stack will be explained in the [Processor Architecture](Processor_Architecture.md) tutorial and an overview on the most common  `Intel x86-64` machine instructions can be found in the following [cheat sheet][X86_CS].

[X86_CS]: https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf

We now set a *breakpoint* at line `11` of the C source code which is just before the sum is updated with the statement `s += n`. Then we start the execution with `run 3` setting the input argument to `n = 3`.
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

[PEDA]: https://github.com/longld/peda

##  List of gdb Shell Commands <a name="section3"></a>

| Command        | Action                                            |
|:---------------|:--------------------------------------------------|
| help [command] | Help information on a command                     |
| break `11`     | Set a breakpoint on line `11`                     |
| run [options]  | Start program execution with optional arguments   |
| continue       | Continue program execution                        |
| step           | Execute next program line                         |
| stepi          | Execute next machine instruction                  |
| info frame     | Information on instruction pointer and stack      |
| info inferiors | IDs of specified inferiors (all if no argument)   |
| info args      | Argument variables of current stack frame         |
| info locals    | Local variables of current stack frame            |
| info threads   | Display currently known threads                   |
| print `n`      | Value of variable `n`                             |
| print/x `&n`   | Memory address of variable `n`                    |
| print/x `$rax` | Value of register `%rax` in hex format            |
| x/i `$rip`     | Value of instruction pointer and next instruction |
| x/8xb `0x7fffffffa0b6` | Show 8 bytes in hex starting from `0x7fffffffa0b6` |
| x/1xg `0x7fffffffdc88` | Show a giant word (8 bytes) in hex starting from `0x7fffffffdc88` |
| x/1s `0x7fffffffde80` | Show a `nul`-terminated string starting from `0x7fffffffde80` |
| quit           | Exit gdb                                         |

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
