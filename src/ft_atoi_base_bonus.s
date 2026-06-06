section .text
	global ft_atoi_base

; Registers used:
;					rax: counting and output
;					rdi: input argument str
;					rsi: input argument base
;					r12: storage of base string length / temporary rdi store
;					r13: storage of sign
; 					rdx: input string offset / temporary index
; 					rcx: current base character / table index
; Prototype: int ft_atoi_base(char *str, char *base);
;
; Converts str into base 10 from whatever is given in base.
; base not only gives the base, but also the chars representing each place value.
;
; The behaviour of libc atoi is matched as best as we can.
; Undefined conditions:
;						If either parameter is NULL.

; Macro signature: CHECK_WHITESPACE address, jump
; Takes 2 parameters: the memory address to check for whitespace,
;	and the label to jump to if it is whitespace.
%macro CHECK_WHITESPACE 2
	cmp		BYTE [%1], 32		; Check for space
	je		%2
	cmp		BYTE [%1], 9		; Check for tab
	je		%2
	cmp		BYTE [%1], 10		; Check for LF
	je		%2
	cmp		BYTE [%1], 11		; Check for VT
	je		%2
	cmp		BYTE [%1], 12		; Check for FF
	je		%2
	cmp		BYTE [%1], 13		; Check for CR
	je		%2
%endmacro

; Macro signature: CHECK_SIGN address, jump_if_plus, jump_if_minus
; Takes 3 parameters: the memory address to check for sign, the label to jump
;	to if +, and the label to jump to if -.
%macro CHECK_SIGN 3
	cmp		BYTE [%1], 43		; Check for +
	je		%2
	cmp		BYTE [%1], 45		; Check for -
	je		%3
%endmacro

; xor reg, reg: Idiomatic way to set a register to 0.
; Fewer bytes and CPU optimised. 32-bit operations implicitly zero upper 32 bits.
errorHappened:
	xor		eax, eax
	jmp		cleanup

return:
	imul	rax, r13			; Apply sign to output, fallthrough to cleanup.

cleanup:
	add		rsp, 256			; Deallocate table from stack
	pop		r13					; Restore callee-saved registers
	pop		r12
	ret

; Save callee-saved registers, allocate 256 bytes on the stack for a table to
;	check base validity and convert characters to values.
; stosb copies AL (0xFF) to [RDI] (RSP), RDI incremented by 1 each time.
; rep repeats stosb RCX (256) times, filling the whole 256 byte table.
; Once done, return RDI to input string pointer, clear RAX and prepare other registers.
ft_atoi_base:
	push	r12					; save callee-saved registers r12 and r13
	push	r13
	sub		rsp, 256			; Allocate 256-bytes on the stack.
	mov		r12, rdi			; Save input string pointer temporarily to r12.
	mov		rdi, rsp			; Destination for stosb: table base
	mov		al, 0xFF			; Clear table with 0xFF sentinel in AL for fill
	mov		ecx, 256			; RCX = 256 bytes for rep to count.
	rep		stosb				; Fill the table buffer at [rsp]
								; REPeat ecx times. Copy al byte to rdi, increment rdi.
	mov		rdi, r12			; Restore input string pointer to rdi.
	xor		eax, eax			; Clear rax for later use.
	xor		r12d, r12d			; Reset base length counter to 0.
	mov		r13, 1				; Set sign tracker to 1.
	jmp		checkBaseLoop

checkBaseIncrement:
	inc		r12

; Check the base for validity.
; Run through using the register r12 as iterator.
; Check for null, check for invalid characters, check for duplicates.
checkBaseLoop:
	cmp		BYTE [rsi + r12], 0			; Have we met string end?
	je		checkBaseFinished			;	If so, jump to next step.
	CHECK_SIGN rsi + r12, errorHappened, errorHappened		; Macros.
	CHECK_WHITESPACE rsi + r12, errorHappened
	movzx	rcx, BYTE [rsi + r12]		; Copy base byte into zero-extended rcx
										;	for use as table index.
	cmp		BYTE [rsp + rcx], 0xFF		; Already in table?
	jne		errorHappened				;	yes: duplicate. Not allowed.
	mov		al, r12b					; Current base index (store raw index)
	mov		[rsp + rcx], al				; Store index in table entry.
	jmp		checkBaseIncrement

checkBaseFinished:
	cmp		r12, 2					; base must have at least 2 characters
	jl		errorHappened
	xor		eax, eax				; Initialise rax and rdx to 0
	xor		edx, edx
	jmp		checkNumberWhitespace

checkNumberWhitespaceIncrement:
	inc		rdx

checkNumberWhitespace:
	cmp		BYTE [rdi + rdx], 0
	je		errorHappened
	CHECK_WHITESPACE rdi + rdx, checkNumberWhitespaceIncrement
	jmp		checkNumberSign

changeSign:
	neg		r13						; Flip sign and fallthrough to increment.

checkNumberSignIncrement:
	inc		rdx

checkNumberSign:
	cmp		BYTE [rdi + rdx], 0
	je		errorHappened
	CHECK_SIGN rdi + rdx, checkNumberSignIncrement, changeSign
	jmp		checkNumberEnd

checkNumberEndIncrement:
	inc		rdx

checkNumberEnd:
	cmp		BYTE [rdi + rdx], 0
	je		return					; End of string, return rax
	CHECK_WHITESPACE rdi + rdx, return
	CHECK_SIGN rdi + rdx, return, return

convertNumber:
	movzx	rcx, BYTE [rdi + rdx]	; Current number byte value.
	movzx	rcx, BYTE [rsp + rcx]	; Look up in table. Stack pointer + char value.
	cmp		rcx, 0xFF				; Set flags based on table value
	je		return					; 0xFF = not in base. Error.
	imul	rax, r12				; Base conversion. r = (r * b) + value
	add		rax, rcx
	jmp		checkNumberEndIncrement

section .note.GNU-stack noexec