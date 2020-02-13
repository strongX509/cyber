# Virtual Memory Management

## Table of Contents
1. [Process Memory Map](#section1)

## Process Memory Map <a name="section1"></a>

We are going to analyze the memory map associated with the `dyn_ctypes` example program used in the [Processor Architecture][PA_HEAP] section.
```assembly
> gdb dyn_ctypes
(gdb) break dyn_incr.c:23
Breakpoint 1 at 0x8f2: file dyn_incr.c, line 23.
(gdb) run
Starting program: /home/andi/cyber/Computing_Systems/dyn_ctypes 
Breakpoint 1, incr (v64=4822678189205112, v32=<optimized out>, v16=52446, v8=<optimized out>, n=<optimized out>) at dyn_incr.c:24
24	}
```
With the `run` command the debugger starts the `dyn_ctypes` process. Its process identifier or `PID` can be determined with the followng `gdb` command:
```assembly
(gdb) info inferior
  Num  Description       Executable        
* 1    process 7028      /home/andi/cyber/Computing_Systems/dyn_ctypes 
```
Because of the breakpoint the `dyn_ctypes` process is waiting to continue. Thus we open a second console terminal and have a look at the `/proc` directory which offers an interface to the kernel. Information on all running processes is available, among them the current *memory map* of the process:
```console
cat /proc/7028/maps
555555554000-555555555000 r-xp 00000000 103:05 4462815  dyn_ctypes  # text & rodata
555555754000-555555755000 r--p 00000000 103:05 4462815  dyn_ctypes
555555755000-555555756000 rw-p 00001000 103:05 4462815  dyn_ctypes  # data & bss
555555756000-555555777000 rw-p 00000000  00:00       0  [heap]
7ffff79e4000-7ffff7bcb000 r-xp 00000000 103:05  437812  /lib/x86_64-linux-gnu/libc-2.27.so
7ffff7bcb000-7ffff7dcb000 ---p 001e7000 103:05  437812  /lib/x86_64-linux-gnu/libc-2.27.so
7ffff7dcb000-7ffff7dcf000 r--p 001e7000 103:05  437812  /lib/x86_64-linux-gnu/libc-2.27.so
7ffff7dcf000-7ffff7dd1000 rw-p 001eb000 103:05  437812  /lib/x86_64-linux-gnu/libc-2.27.so
7ffff7dd1000-7ffff7dd5000 rw-p 00000000  00:00       0 
7ffff7dd5000-7ffff7dfc000 r-xp 00000000 103:05  393251  /lib/x86_64-linux-gnu/ld-2.27.so
7ffff7fd5000-7ffff7fd7000 rw-p 00000000  00:00       0 
7ffff7ff7000-7ffff7ffa000 r--p 00000000  00:00       0  [vvar]
7ffff7ffa000-7ffff7ffc000 r-xp 00000000  00:00       0  [vdso]
7ffff7ffc000-7ffff7ffd000 r--p 00027000 103:05  393251  /lib/x86_64-linux-gnu/ld-2.27.so
7ffff7ffd000-7ffff7ffe000 rw-p 00028000 103:05  393251  /lib/x86_64-linux-gnu/ld-2.27.so
7ffff7ffe000-7ffff7fff000 rw-p 00000000  00:00       0 
7ffffffde000-7ffffffff000 rw-p 00000000  00:00       0  [stack]
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0 [vsyscall]
```
We see that the *memory map* of process `7028` consists of 17 user space entries and one kernel entry, some of them with a size of 4096 bytes and some of them larger. We can compare this map with the [Virtual Memory Map][PA_MAP] that we constructed earlier for the `dyn_ctypes` program based on `gdb` information:

![Virtual Memory Map][PA_MAP_IMG]

* `0x555555554000`  contains the read-only and executable `text` and `rodata` segments 
* `0x555555755000`  contains the  writable but not executable `data` and `bss` segments
* `0x555555756000` contains the writable but not executable *Heap* segment
* `0x7ffffffde000` contains the writable but not executable *Stack* segment

Detailed information on each `maps` entry can be extracted from `smaps`:
```console
cat /proc/7028/smaps
...
7ffffffde000-7ffffffff000 rw-p 00000000 00:00 0  [stack]
Size:                132 kB
KernelPageSize:        4 kB
MMUPageSize:           4 kB
Rss:                   8 kB
Pss:                   8 kB
Shared_Clean:          0 kB
Shared_Dirty:          0 kB
Private_Clean:         0 kB
Private_Dirty:         8 kB
Referenced:            8 kB
Anonymous:             8 kB
LazyFree:              0 kB
AnonHugePages:         0 kB
ShmemPmdMapped:        0 kB
Shared_Hugetlb:        0 kB
Private_Hugetlb:       0 kB
Swap:                  0 kB
SwapPss:               0 kB
Locked:                0 kB
VmFlags: rd wr mr mw me gd ac
...
```
We see from the `Rss` field of the `stack` entry that only two pages of physical memory (8 kB of RAM) of the 132 kB *Stack* segment have been allocated.

[PA_HEAP]: Processor_Architecture.md#section5
[PA_MAP]:  Processor_Architecture.md#section6
[PA_MAP_IMG]: Memory_Map_200.png

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
