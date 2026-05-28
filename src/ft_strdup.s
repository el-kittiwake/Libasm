section .text
	global ft_strdup
	extern ft_strlen
	extern ft_strcpy
	extern malloc

; Registers in use:
;					rax: output, counter
;					rdi: input string pointer (*s)
; prototype: char *strdup(const char *s)
;
; Like with libft I will calculate length, mallocate memory and copy s to that memory.

ft_strdup:
	mov		rax, 0			; Initialise accumulator to 0

ft_strlenCall:
	push	rdi				; Push rdi to stack to save for future
	call	ft_strlen		; Call ft_strlen() with rdi as parameter, output to rax

mallocCall:
	mov		rdi, rax			; Move length count to rdi. malloc() parameter
	inc		rdi					; Increment rdi to account for \0 terminator
	call	malloc wrt ..plt	; Call malloc(), if successful pointer to memory
								;	is in rax. If not, rax is zero.
								;	Using wrt ..plt again to prevent the PIE issue.
	pop		rdi					; Restore rdi as we are done with it
	cmp		rax, 0				; Check malloc result for null
	je		mallocError			; If null, jump to error handling

ft_strcpyCall:
	mov		rsi, rdi			; Move rdi to rsi to satisfy ft_strcpy's params
	mov		rdi, rax			; Move mallocated address to rdi
	call	ft_strcpy			; Call ft_strcpy with rdi and rsi. Return to rax

finished:
	ret							; Return rax

mallocError:
	mov		rax, 0				; On error set rax to 0. malloc() itself sets errno.
	ret							; Return rax
