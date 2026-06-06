section .text
	global ft_strdup
	extern ft_strlen
	extern malloc

; Registers in use:
;			rax: output / malloc return value / ft_strlen return value
;			rdi: input pointer (*s) / malloc size param / destination pointer (*return)
;			rsi: source pointer
;			rcx: loop counter
; prototype: char *strdup(const char *s)
;
; Like with libft I will calculate length, mallocate memory and copy s to that memory.

ft_strdup:
	push	rdi					; Push rdi to stack to save through calls.

ft_strlenCall:
	call	ft_strlen			; Call ft_strlen, rdi is parameter, output to rax.
	push	rax					; Push rax to stack to save through calls.

mallocCall:
	mov		rdi, rax			; Move length count to rdi. malloc() parameter
	inc		rdi					; Increment rdi to account for \0 terminator
	sub		rsp, 8				; Align stack to 16 bytes for the call
	call	malloc wrt ..plt	; Call malloc(), if successful pointer to memory
								;	is in rax. If not, rax is zero.
								;	Using wrt ..plt again to prevent the PIE issue.
	add		rsp, 8				; Restore stack alignment after call.
	cmp		rax, 0				; Check malloc result for null.
	je		mallocError			; If null, jump to error handling.

copyPrep:
	pop		rcx					; Pop the length to rcx, loop counter parameter.
	pop		rsi					; Move original string pointer to rsi for copying.
	push	rax					; Push the malloc returned pointer back to stack.
	mov		rdi, rax			; Move malloc returned pointer to rdi for copying.
	test	rcx, rcx			; Test if string length is zero (but not null).
	je		done				; If length is zero, skip copy loop.

copyLoop:
	mov		dl, [rsi]			; Copy byte from source (rsi) to dest (rdi) via dl.
	mov		[rdi], dl
	inc		rsi					; Increment source and destination pointers
	inc		rdi
	loop	copyLoop			; Loop until rcx is zero.
done:
	mov		byte [rdi], 0		; Set null terminator at string end.
	pop		rax					; Restore return pointer.
	ret

mallocError:
	add		rsp, 16				; Remove saved original pointer and saved length.
	mov		rax, 0				; On error set rax to 0. malloc() itself sets errno.
	ret							; Return rax

section .note.GNU-stack noexec