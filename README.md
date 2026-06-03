# Libasm

Subject version 5.1

"Summary: The aim of this project is to become familiar with assembly language."

## Assembly language

Assembly languages are a family of low-level programming languages. Low-level meaning that they sit one step up from raw machine code.

Each instruction in an assembly language (`mov`, `cmp`, `jmp` etc.) is essentially a human-readable connection to processor opcodes (the binary operations of machine code) and operands (the binary encoding of registers, memory addresses etc.).

Because of this intimate relationship with specific hardware, assembly is hardware specific. x86 assembly won't work with Z80 hardware etc.

The main advantage of using assembly is that it gives the programmer precise, specific control of the hardware, what to do with specific registers and memory and how to get the processor to behave on a very granular level.

Higher-level programming languages such as C, Fortran, Python and so on trade this precise control for compatibility over a wider range of architectures. They lose performance, but they gain the ability to "easily" move programs between different systems.

## The task at hand

According to the subject we have to recreate several libc functions in assembly language.

Basic requirements:

- Architecture: [x86-64](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- Calling convention: [System V ABI](https://www.sco.com/developers/gabi/latest/contents.html)
- Syntax: [Intel](https://en.wikipedia.org/wiki/X86_assembly_language#Syntax)
- Assembler: [NASM (The Netwide Assembler)](https://www.nasm.us/docs/3.01/)
- Filetype: .s

Using NASM forces Intel syntax anyway.

### Calling convention

The calling convention is a set of rules that the CPU and assembler adhere to.

In this work, the calling convention affects the following:

- Where arguments go
  - In our case the first four arguments go to: `rdi`, `rsi`, `rdx` and `rcx`.
- Where the return value goes
  - In our case `rax` is returned
- Which registers behave in specific ways
- Caller-saved vs. callee-saved registers. 
  - Caller-saved: the called function doesn't need to preserve the value of the register. Our four argument registers are this type.
  - Callee-saved: the called function must restore the register's value before returning. This is a *gentleman's agreement* that the programmer must oblige.

### Syntax

In the x86-64 world there are two main syntaxes, Intel and AT&T. There are two main differences between them.

- Operand order: Intel has the destination first, source second. AT&T vice versa.
- Decoration: AT&T adds prefixes to registers (%) and immediate values ($).
- Size specification: AT&T adds a suffix to the instruction indicating the register size (e.g. d for double word). Whereas Intel syntax requires an explicit size (e.g. DWORD for double word) only when the operand width is not clear, especially for memory operands.

Example using the move instruction.

``` asm
mov   rax, 1      ; (Intel)
movq  $1,  %rax   ; (AT&T)
```

## Mandatory

Recreate: strlen, strcpy, strcmp, write, read, strdup (malloc call permitted)

Library: libasm.a

It is required to submit a main to test these functions.

Where relevant `errno` must be set and syscall errors must be handled appropriately.

Calling of __errno_location is permitted.

Basic functionality of each function must match that of libc and the man page.


### The functions

Implementation is in code. This section is for notes on new concepts and other interesting things.


#### ft_strlen

Prototype: `size_t strlen(const char *s);`

##### Idea

Loop through the given string counting bytes. Return count.

Undefined behaviours:

- String is not `\0` terminated.
- NULL pointer passed.
- Inaccessible memory space passed.

##### New concepts

Everything at this point is new.

`; comments`: Comments are preceded by semicolons.

`section .text`

- `section`: is a way for code to be organised into segments.
- `.text`: is the section label for actual code.

`global ft_strlen`

- `global`: makes the given function name visible to the linker
- `ft_strlen`: is the only label that will be visible to the linker

`ft_strlen:`: a label that can be used to jump to or in this case as a global export

###### Registers

x86-64 has 16 general purpose registers: `rax`, `rbx`, `rcx`, `rdx`, `rsi`, `rdi`, `rsp`, `rbp`, and `r8` to `r15`. These are all 64-bits (8 bytes) wide.

These registers can hold both addresses and program data.

Most instructions operate on these registers either directly on the data they store or on the data the address they store points to.

`mov rax, 0`

- `mov`: move data instruction (actually copies from source to destination, leaving the source unchanged)

###### Register bytes

In addition to operating on the full 64-bit register, instructions can operate on smaller portions of it. Data widths of byte (8-bit), word (16-bit), doubleword (32-bit) and quadword (64-bit) are supported and can be stored in the register.

Size specifiers are used to resolve ambiguity when NASM cannot infer the operation width from context. This arises primarily with memory operands, where no register is present to imply the size.

These specifiers are: `BYTE` (8-bit), `WORD` (16-bit), `DWORD` (32-bit), `QWORD` (64-bit). Examples below:

```asm
mov     al, [rdi]           ; register implies 8-bit. No specifier needed.
mov     BYTE [rdi], 5       ; memory operand only. Specifier required.
cmp     BYTE [rdi + rax], 0 ; memory operand only. Specifier required.
```

The 16 general purpose registers can be specifically referenced to the above sizes.

**RAX as an example**

| Width      | Name  | Covers |
|------------|-------|--------|
| 64-bit     | `rax` | bytes 7–0 (all 8 bytes) |
| 32-bit     | `eax` | bytes 3–0 (lower half) |
| 16-bit     | `ax`  | bytes 1–0 (lowest 2 bytes) |
| 8-bit high | `ah`  | byte 1 |
| 8-bit low  | `al`  | byte 0 |

**Equivalents for all x86 registers**

| 64-bit | 32-bit | 16-bit | 8-bit low | 8-bit high |
|--------|--------|--------|-----------|------------|
| `rax`  | `eax`  | `ax`   | `al`      | `ah`       |
| `rbx`  | `ebx`  | `bx`   | `bl`      | `bh`       |
| `rcx`  | `ecx`  | `cx`   | `cl`      | `ch`       |
| `rdx`  | `edx`  | `dx`   | `dl`      | `dh`       |
| `rsi`  | `esi`  | `si`   | `sil`     | —          |
| `rdi`  | `edi`  | `di`   | `dil`     | —          |
| `rsp`  | `esp`  | `sp`   | `spl`     | —          |
| `rbp`  | `ebp`  | `bp`   | `bpl`     | —          |
| `r8`   | `r8d`  | `r8w`  | `r8b`     | —          |
| `r9`   | `r9d`  | `r9w`  | `r9b`     | —          |
| `r10`  | `r10d` | `r10w` | `r10b`    | —          |
| `r11`  | `r11d` | `r11w` | `r11b`    | —          |
| `r12`  | `r12d` | `r12w` | `r12b`    | —          |
| `r13`  | `r13d` | `r13w` | `r13b`    | —          |
| `r14`  | `r14d` | `r14w` | `r14b`    | —          |
| `r15`  | `r15d` | `r15w` | `r15b`    | —          |

Writing to a 32-bit variant automatically zeros out the upper 32-bits of the full 64-bits. Any value previously held in those upper bits is lost.

A register given inside a pair of square brackets eg. `[rax]` is "dereferenced", meaning it points to the contents of the address stored in the register. Operations performed on dereferenced registers is performed on the data it is pointing to.

Without brackets any operations will see the data actually stored in the register itself (address, counter, so on).

Arithmetic can be performed on registers used as operands for instructions. `rax + 1` adds 1 to the value held in rax. `rdi + rax` adds rax to rdi. `[rdi + rax]` will point to the data held at address rdi plus rax.

`cmp BYTE [rdi + rax], 0`

- `cmp`: compare values. Performs a subtraction but discards the result, updates the RFLAGS register based on what the result would have been. These flags can be used later to control program flow.
- `[rdi + rax]`: `rdi` register (first argument) added to the current value of the `rax` counter register. This combined with `BYTE` ensures that the count marches along the input string bytewise.
- `0`: If a zero (null) byte is found, the end of the string has been reached.

###### Jumps

After a comparison operation has been performed it is generally desired to do something with the result. A common action is to jump to a part of the code depending on the result.

`jmp` is a common jump that will move execution to the label, register or memory address given as its operand. However there are numerous other jumps that depend on the results of comparison operations.

###### Common x86 jumps

A list of jumps relevant to this project, plus a few others of interest.

Flag refers to the data stored in the RFLAGS register.

**Equality**

| Mnemonic | Meaning | Flag |
|----------|---------|------|
| `je` / `jz` | Jump if equal / zero | ZF = 1 |
| `jne` / `jnz` | Jump if not equal / not zero | ZF = 0 |

**Signed comparisons**

| Mnemonic | Meaning | Flags |
|----------|---------|-------|
| `jg` / `jnle` | Jump if greater | ZF = 0, SF = OF |
| `jge` / `jnl` | Jump if greater or equal | SF = OF |
| `jl` / `jnge` | Jump if less | SF ≠ OF |
| `jle` / `jng` | Jump if less or equal | ZF = 1 or SF ≠ OF |

**Unsigned comparisons**

| Mnemonic | Meaning | Flags |
|----------|---------|-------|
| `ja` / `jnbe` | Jump if above | CF = 0, ZF = 0 |
| `jae` / `jnb` | Jump if above or equal | CF = 0 |
| `jb` / `jnae` | Jump if below | CF = 1 |
| `jbe` / `jna` | Jump if below or equal | CF = 1 or ZF = 1 |

**Flag checks**

| Mnemonic | Meaning | Flag |
|----------|---------|------|
| `jo` | Jump if overflow | OF = 1 |
| `jno` | Jump if no overflow | OF = 0 |
| `js` | Jump if sign (negative) | SF = 1 |
| `jns` | Jump if no sign (positive) | SF = 0 |
| `jc` | Jump if carry | CF = 1 |
| `jnc` | Jump if no carry | CF = 0 |
| `jp` / `jpe` | Jump if parity even | PF = 1 |
| `jnp` / `jpo` | Jump if parity odd | PF = 0 |

`je finished`

- `je`: jump if equal. Jumps to its operand if the previous comparison resulted in equality.
- `finished`: the label to jump to.

###### Increment and decrement

| Instruction | Example | Effect |
|-------------|---------|--------|
| `inc` | `inc rax` | `rax = rax + 1` |
| `dec` | `dec rax` | `rax = rax - 1` |

`ret`: return. By convention, the caller reads `rax` as the result.

#### ft_strcpy

Prototype: `char *strcpy(char *restrict dst, const char *restrict src);`

##### Idea

Loop through the given source string copying bytewise to destination string. Return pointer to destination.

Undefined behaviours:

- Source and destination overlap.
- String is not `\0` terminated.
- Destination is not large enough for source.
- Source or destination is NULL or invalid.

#### ft_strcmp

Prototype: `int strcmp(const char *s1, const char *s2);`

##### Idea

Loop through the given strings (s1 & s2) comparing bytewise. If any difference is found at a given position then return the value of s2 subtracted from s1. If the end of the string is reached with no differences return 0.

Undefined behaviours:

- s1 and s2 overlap.
- s1 or s2 are not `\0` terminated.
- Either string is NULL or invalid.

##### New concepts

- `movzx`: move with zero extension. Moves a value to a larger register and fills upper bits with zeroes. Used when source is unsigned.
- `movsx`: move with sign extension. Similar to its sibling movzx, but extends the sign bit to fill the upper bits. Used when source is signed.

Both of these instructions require that size prefixes are used, even if the size seems like it can be inferred from context.

###### Basic mathematical functions

| Instruction | Example | Effect |
|-------------|---------|--------|
| `add` | `add rax, rbx` | `rax = rax + rbx` |
| `sub` | `sub rax, rbx` | `rax = rax - rbx` |
| `imul` (signed) | `imul rax, rbx` | `rax = rax * rbx` |
| `mul` (unsigned) | `mul rbx` | `rdx:rax = rax * rbx` |
| `idiv` (signed) | `idiv rbx` | `rax = rdx:rax / rbx`, `rdx = remainder` |
| `div` (unsigned) | `div rbx` | `rax = rdx:rax / rbx`, `rdx = remainder` |
| `neg` | `neg rax` | `rax = -rax` |

In order to handle overflow, `mul` and the single-operand form of `imul` store the full double-width result across `rdx:rax`. By placing the upper half in `rdx` and the lower in `rax`.

Division always operates on `rdx:rax` as the dividend. `rdx` must be set up correctly before dividing.

#### ft_write

Prototype: `ssize_t write(int fd, const void buf[.count], size_t count);`

##### Idea

Make a syscall to sys_write (ID: 1) with the arguments passed to the function. If successful return the number of bytes written. On error, return -1 and set errno.

##### New concepts

`syscall`: runs a standard system command. Its parameters are specific registers set before calling. For everything we do in this project the registers `rax`, `rdi`, `rsi` and `rdx` will be all that are used.

###### Error checking (errCheck)

`-4095`: The limit of the error number range in Linux. All errnos have to fit within -1 and -4095.

`jbe`: jump if below or equal. This is the unsigned version of `jle`.

Initially I was thinking to simply check if the `syscall` return in `rax` was negative and then follow the setError path. However, after some reading about how Linux handles errors it seems that this can lead to false results for some edge cases.

The simple negative check would likely work fine for a sys_write call, however for calls that return pointers it could lead to false positives. This is because in the upper addresses the sign bit is set and a signed check would return negative in that case.

Using the -4095 number simply checks that the errno is within the expected error range. The kernel reserves this small range strictly for error number checking.

More information is available in the [GNU C documentation](https://sourceware.org/glibc/manual/latest/html_node/Error-Reporting.html) the [Linux Standard Base 5.0 specification](http://refspecs.linux-foundation.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib---errno-location.html) and is mentioned briefly in the [Linux source](https://github.com/torvalds/linux/blob/master/tools/include/linux/err.h)

Documentation is really sparse for this sort of specific thing. There are reams of information about how to write assembly of various different flavours. But finding system specific information is more of a challenge and is spread out over more sources.

###### Callee saved registers

``` asm
; Within setError:
push  r13
    ...
pop   r13
```

The above code snippet seems pointless. However, I am using a callee-saved register (lucky `r13`) to store the error number value from `rax` before using `rax` for another call.

Callee-saved registers require the programmer to save their value and reapply it after I have finished with the register. Regardless of if I use the register for anything other than brief storage.

This snippet is only needed in the setError section because r13 will only ever be used if an error needs setting.

Assembly has a few things like this, that seem kind of "pointless" but are necessary under certain circumstances, so we do them under all circumstances.

###### External functions

`extern  __errno_location`

- `extern`: tells the linker to expect a symbol from outside of the current file. The linker will find it from glibc.
- `__errno_location`: is the function that returns the pointer to the current thread's errno location in memory. It outputs to rax, as is standard.

`call __errno_location wrt ..plt`

- `call`: calls the function given as first operand. Call differs from `jmp` in that it executes code it is linked to then returns to where the call was made. It changes the call stack before executing the function. It is used to run code that returns something back.
- `wrt`: "with respect to". A NASM specific operator that says "calculate this address relative to something".
- `..`: denotes a NASM built in symbol rather than a user defined label.
- `plt`: "Procedure Linkage Table". A section the linker adds to the binary. Essentially a lookup table for external functions. Libraries are loaded to random locations in memory, so the binary needs a way to know where to find them. The PLT directs the execution to check dynamically where the address (of __errno_location in this case) is. This is stored, so future calls go straight there.

#### ft_read

Prototype: `ssize_t read(int fd, void buf[.count], size_t count);`

This function is the same as ft_write. The only difference is that it uses syscall ID 0 instead of ID 1.

#### ft_strdup

Prototype: `char *strdup(const char *s);`

##### Idea

Duplicate the string into heap allocated memory. For this I use three external functions. ft_strlen(), ft_strcpy() and malloc().

If the memory allocation fails then malloc() returns null and sets errno.

Undefined behaviours:

- String is not `\0` terminated.
- String pointer is NULL or invalid.

##### New concepts

###### Using custom functions as externals

This is very much like using system functions as externals. I use ft_strlen() and ft_strcpy() from the libasm.a library.

``` asm
extern  ft_strlen
extern  ft_strcpy
    ...
call    ft_strlen
    ...
call    ft_strcpy
```

###### Testing malloc() failure with LD_PRELOAD

Using a shared library that temporarily replaces malloc(). The following C code "replaces" malloc.

``` c
// fake_malloc.c
#include <stdlib.h>
#include <errno.h>

void *malloc(size_t size)
{
    errno = ENOMEM;
    return NULL;
}
```

Compile the library and then run the test program.

``` bash
gcc -shared fake_malloc.c -o fake_malloc.so
LD_PRELOAD=./fake_malloc.so ./test.out
```

`LD_PRELOAD` tells the linker to load the `fake_malloc.so` shared object library before any others. Anything defined in that library replaces the system standard for the duration of the programs execution.

In the above case it replaces malloc(), forcing every allocation to fail with `ENOMEM`. This allows for NULL and errno testing without needing to artificially exhaust system memory.

## Bonus

Recreate: ft_atoi_base, ft_list_push_front, ft_list_size, ft_list_sort, ft_list_remove_if

These are all 42-school specific functions. The linked list functions are the same as some of those found in module C12 of the Piscine and similar to those in the bonus part of libft. The ft_atoi_base function is similar to a function found in the rank 2 exam, level 3.

Linked lists must be of the following format:

``` C
typedef struct s_list
{
    void            *data;
    struct  s_list  *next;
}        t_list;
```

I can only assume that the mandatory rules also apply to the bonuses as well.

### The functions

I only attempted ft_atoi_base() as it seemed the most awkward to implement of the bonus functions.

#### ft_atoi_base

Prototype: `int ft_atoi_base(char *str, char *base);`

##### Idea

This function performs a similar function to libc atoi(), it converts a given string (*str) into an integer.

Where it differs is that it takes a second parameter, another string which represents an arbitrary base (*base). This is a string which contains the characters used to represent each number. The characters always represent numbers from 0 to n.

Example: value: ^zta321 base: 123atz^ return in base10: 800667.

In this example, 1 represents 0 and ^ represents 7.

The return will be in base 10 with the usual 0 - 9 characters.

The base cannot contain duplicate characters, it also cannot contain the characters - or +.

##### New concepts

Quite a few new concepts cropping up in this task.

###### Macros

Assembly only has labels and jumps to allow the execution flow to break away from linewise execution at runtime. Macros are a preprocessor mechanic to give assembly a text substitution and code reuse mechanism.

There are two forms in NASM:

- `%define`: A simple substitution. During preprocessing, \<name\> is replaced by \<value\> wherever it appears in code.
  - `%define <name> <value>`

- `%macro`: A potentially multi-line parameterised block. Parameters are referenced as `%1`, `%2`, etc.

``` asm
%macro <name> <num_params>
    cmp  %1, 24
    je   %2
    cmp  %1, 42
    je   %3
        ...
    cmp  %1, 69
    je   %n
%endmacro
```

Assembly also has the `EQU` operator. This is used for true constants and has global scope. The assembler enforces its immutability.

\<name\> is assembled as a symbol with the value \<value\>.

`<name> EQU <value>`

###### Stack allocation

Implementation of this function uses a character lookup table that is populated with values. This turns what would be an O(n) linear sweep through the base string (per character) into an O(1) table lookup at the meagre cost of 256 bytes of stack space.

Each index of this table represents an ASCII character and is one byte. The value held in that index represents the numeric value of that character.

The lookup table is initialised with the value FF, a value that cannot realistically be a legitimate numeric value.

Process:

1. Make 256-bytes of space on the stack below callee-saved values etc.
2. Preserve registers that are needed for instruction calls.
3. Set registers:
   - rdi: destination address (top of stack at first)
   - al (rax lowest byte): character to fill with
   - rcx: size in bytes to fill (number of times to repeat)
4. repeatedly call *store string* `stosb` using `rep`
   - After each write the value of rdi is incremented by one
5. Revert parameter registers to their pre call state.

The below illustrates the condition of the stack after allocation and the condition of the lookup table at arbitrary points.

```
STACK (after prologue)                LOOKUP TABLE  [rsp .. rsp+255]
                                      (one byte per ASCII code point)
  high                                 offset: 0x00        0x30 0x31      0x41    0xFF
   |  [ return addr ]                          |            |    |          |       |
   |  [  saved rbp  ]                  value: [FF  FF  ... 00   01  ... 0A  FF ... FF]
   |  [ saved regs  ]                          ^            ^    ^          ^
   |  [ 256B table  ] <-- rsp              invalid/        '0'  '1'        'A'
  low                                      sentinel
       ^
       sub rsp, 256     <-- flat slab, no malloc, cleaned on return
```

Two pertinent new instructions are used in this process.

- `rep`: repeats its operand the number of times stored in register `rcx`.
- `stosb`: stores byte value in register `al` to location in register `rdi`. Increments `rdi` by one after it completes.
