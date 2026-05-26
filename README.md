# Libasm

Subject version 5.1

"Summary: The aim of this project is to become familiar with assembly language."

## Assembly language

Assembly languages are a family of low-level programming languages. Low-level meaning that they sit one step up from raw machine code.

Each instruction in an assembly language (`mov`, `cmp`, `jmp` etc.) is essentially a human readable connection to processor opcodes (the binary operations of machine code) and operands (the binary encoding of registers, memory addresses etc.).

Because of this intimate relationship with specific hardware, assembly is hardware specific. x86 assembly won't work with Z80 hardware etc.

The main advantage of using assembly is that it gives the programmer precise, specific control of the hardware, what to do with specific registers and memory and how to get the processor to behave on a very granular level.

Higher level programming languages such as C, Fortran, Python and so on trade this precise control for compatibility over a wider range of architectures. They lose performance, but they gain the ability to "easily" move programs between different systems.

## The task at hand

According to the subject we have to recreate several libc functions in assembly language.

Basic requirements:

- Architecture: x86-64
- Calling convention: [System V ABI](https://www.sco.com/developers/gabi/latest/contents.html)
- Syntax: [Intel](https://en.wikipedia.org/wiki/X86_assembly_language#Syntax)
- Assembler: [NASM (The Netwide Assembler)](https://www.nasm.us/docs/3.01/)
- Filetype: .s

Using NASM forces Intel syntax anyway.

#### Calling convention

The calling convention is a set of rules that the CPU and assembler adhere to.

In this work, the calling convention affects the following:

- Where arguments go
	- In our case the first four arguments go to: `rdi`, `rsi`, `rdx` and `rcx`.
- Where the return value goes
	- In our case `rax` is returned
- Which registers behave in specific ways
	- Caller-saved vs. callee-saved registers. 
		- Caller-saved: the called function doesn't need to preserve the value of the register. Our four argument registers are this type.
		- Callee-saved: the called function must restore the register's value before returning.

#### Syntax

In the x86-64 world there are two main syntaxes, Intel and AT&T. There are two main differences between them.

- Operand order: Intel has the destination first, source second. AT&T vice versa.
- Decoration: AT&T adds prefixes to registers (%) and immediate values ($). Suffixes indicating the size are added to instructions (eg. q for quad word).

Example using the move instruction.

```
mov rax, 1	(Intel)
movq $1, %rax	(AT&T)
```

### Mandatory

Recreate: strlen, strcpy, strcmp, write, read, strdup (malloc call permitted)

Library: libasm.a

It is required to submit a main to test these functions.

Where relevant `errno` must be set and syscall errors must be handled appropriately.

Calling of ___error or errno_location is permitted.

Basic functionality of each function must match that of libc and the man page.

## The functions

Implementation is in code. This section is for notes on new concepts and other interesting things.

### ft_strlen

#### Idea

Loop through the given string counting bytes. Return count.

Undefined behaviours:

- String is not `\0` terminated.
- NULL pointer passed.
- Inaccessible memory space passed.

#### New concepts

Everything at this point is new.

`section .text`

- `section`: is a way for code to be organised into segments.
- `.text`: is the section label for actual code.

`global ft_strlen`

- `global`: makes the given function name visible to the linker
- `ft_strlen`: is the only label that will be visible to the linker

`ft_strlen`: a label that can be used to jump to or in this case as a global export

`mov rax, 0`

- `mov`: move data instruction (actually copies from source to destination, leaving the source unchanged)
- `rax`: "Accumulator" register, default return register. Used here as a counter.

`cmp BYTE [rdi + rax], 0`

- `cmp`: compare values. Store the result of subtracting source from destination in the RFLAGS register.
- `BYTE`: read the byte at the given address. This reads the first byte of the 8 byte address.
- `[rdi + rax]`: `rdi` register (first argument) added to the current value of the `rax` counter register. This combined with `BYTE` ensures that the count marches along the input string bytewise.
- `[ ] notation`: like with C, we have pointers and dereferencing. This means the contents of the address inside the brackets.
- `0`: If a zero (null) byte is found, the end of the string has been reached.

`je finished`

- `je`: jump if equal. Jumps to its operand if the previous comparison resulted in equality.
- `finished`: the label to jump to.

`inc rax`: increment the value of `rax`. rax += 1.

`jmp`: a plain jump to the label given as operand.

`ret`: return. Returns `rax` as result.

### ft_strcpy

#### Idea

Loop through the given source string copying bytewise to destination string. Return pointer to destination.

Undefined behaviours:

- Source and destination overlap.
- String is not `\0` terminated.
- Destination is not large enough for source.
- Source or destination is NULL or invalid.

#### New concepts

- `cl`: This is the lowest byte of the full 64-bit register `rcx`. This register is used as a middle-man. x86-64 does not allow copying between memory locations.
- `rsi`: Second argument register.

### ft_strcmp

#### Idea

Loop through the given strings (s1 & s2) comparing bytewise. If any difference is found at a given position then return the value of s2 subtracted from s1. If the end of the string is reached with no differences return 0.

Undefined behaviours:

- s1 and s2 overlap.
- s1 or s2 are not `\0` terminated.
- Either string is NULL or invalid.

#### New concepts

- `jne`: jump if not equal. Jumps to its operand if the previous comparison resulted in inequality.
- `movzx`: move with zero extension. Moves a value to a larger register and fills upper bits with zeroes.
- `eax, ecx`: the lower 32-bits of the rax and rcx registers respectively.
- `sub`: subtract second operand from first, place result in first.
