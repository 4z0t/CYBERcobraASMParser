# CYBERcobraASMParser

## Overview

This program is designed to convert most of the ASM instructions to [CYBERcobra](https://github.com/MPSU/APS/tree/%D0%9F%D0%98%D0%9D/Labs/4.%20Primitive%20programmable%20device) ones.

Specifically `x86-64` ASM. Here on [Godbolt](https://godbolt.org/) choose `x86-64 gcc 12.2` compiler. Now we can write a simple program on **C/C++** like this one:

```cpp
typedef unsigned int uint;
uint f()
{
    uint i = -1;
    uint a = 4;
    if(a>=i)
    return a+i/2;
    return i+a/2;
}
```

Be sure to not use any optimizations, since this totally changes how code works.

We get this:

```asm
f():
        push    rbp
        mov     rbp, rsp
        mov     DWORD PTR [rbp-4], -1
        mov     DWORD PTR [rbp-8], 4
        mov     eax, DWORD PTR [rbp-8]
        cmp     eax, DWORD PTR [rbp-4]
        jb      .L2
        mov     eax, DWORD PTR [rbp-4]
        shr     eax
        mov     edx, eax
        mov     eax, DWORD PTR [rbp-8]
        add     eax, edx
        jmp     .L3
.L2:
        mov     eax, DWORD PTR [rbp-8]
        shr     eax
        mov     edx, eax
        mov     eax, DWORD PTR [rbp-4]
        add     eax, edx
.L3:
        pop     rbp
        ret
```

And then we cut off function stack stuff, since it is not implemented yet:

```asm
        mov     DWORD PTR [rbp-4], -1
        mov     DWORD PTR [rbp-8], 4
        mov     eax, DWORD PTR [rbp-8]
        cmp     eax, DWORD PTR [rbp-4]
        jb      .L2
        mov     eax, DWORD PTR [rbp-4]
        shr     eax
        mov     edx, eax
        mov     eax, DWORD PTR [rbp-8]
        add     eax, edx
        jmp     .L3
.L2:
        mov     eax, DWORD PTR [rbp-8]
        shr     eax
        mov     edx, eax
        mov     eax, DWORD PTR [rbp-4]
        add     eax, edx
.L3:
```

CYBERcobra supports loading values from other devices, lets change it:

```asm
        read    DWORD PTR [rbp-4]
        mov     DWORD PTR [rbp-8], 4
        mov     eax, DWORD PTR [rbp-8]
        cmp     eax, DWORD PTR [rbp-4]
        jb      .L2
        mov     eax, DWORD PTR [rbp-4]
        shr     eax
        mov     edx, eax
        mov     eax, DWORD PTR [rbp-8]
        add     eax, edx
        jmp     .L3
.L2:
        mov     eax, DWORD PTR [rbp-8]
        shr     eax
        mov     edx, eax
        mov     eax, DWORD PTR [rbp-4]
        add     eax, edx
.L3:
```

Now we read it on place of `i` that had `-1` before.

Add last op too:

```asm
        read    DWORD PTR [rbp-4]
        mov     DWORD PTR [rbp-8], 4
        mov     eax, DWORD PTR [rbp-8]
        cmp     eax, DWORD PTR [rbp-4]
        jb      .L2
        mov     eax, DWORD PTR [rbp-4]
        shr     eax
        mov     edx, eax
        mov     eax, DWORD PTR [rbp-8]
        add     eax, edx
        jmp     .L3
.L2:
        mov     eax, DWORD PTR [rbp-8]
        shr     eax
        mov     edx, eax
        mov     eax, DWORD PTR [rbp-4]
        add     eax, edx
.L3:
        mov     eax, eax
```

Fun part now. After passing it through program we get this:

```
0       0 0 10 00000 00000 00000 00000000 10001 20000011        0       [17] <- sw_i
1       0 0 00 00000000000000000000100 10010    00000092        1       [18] <- 4
2       0 0 01 00000 10010 00000 00000000 00001 10480001        2       [1] <- [18]
3       0 1 00 11110 00001 10001 00001000 00000 4f062100        3       J 11 if [ 1] <u [17]
4       0 0 01 00000 10001 00000 00000000 00001 10440001        4       [1] <- [17]
5       0 0 00 00000000000000000000001 00111    00000027        5       [7] <- 1
6       0 0 01 00101 00001 00111 00000000 00001 1284e001        6       [1] <- [ 1] >> [ 7]
7       0 0 01 00000 00001 00000 00000000 00100 10040004        7       [4] <- [ 1]
8       0 0 01 00000 10010 00000 00000000 00001 10480001        8       [1] <- [18]
9       0 0 01 00000 00001 00100 00000000 00001 10048001        9       [1] <- [ 1] + [ 4]
10      1 0 00 00000 00000 00000 00000111 00000 800000e0        10      J 17
11      0 0 01 00000 10010 00000 00000000 00001 10480001        11      [1] <- [18]
12      0 0 00 00000000000000000000001 00111    00000027        12      [7] <- 1
13      0 0 01 00101 00001 00111 00000000 00001 1284e001        13      [1] <- [ 1] >> [ 7]
14      0 0 01 00000 00001 00000 00000000 00100 10040004        14      [4] <- [ 1]
15      0 0 01 00000 10001 00000 00000000 00001 10440001        15      [1] <- [17]
16      0 0 01 00000 00001 00100 00000000 00001 10048001        16      [1] <- [ 1] + [ 4]
17      0 0 01 00000 00001 00001 00000000 00001 10042001        17      [1] <- [ 1] + [ 1]
```

You can see binary and hex representations of our code on the left and pseudo code on the right, note that registers mess up some operations a bit, but I hope I'll fix it soon :P

**No** there is no Hello world here unfortunately, however...

