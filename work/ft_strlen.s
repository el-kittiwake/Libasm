section .text
	global ft_strlen

; Registers in use:
;					rax: counting and output
;					rdi: input argument (x86-64 System V ABI calling convention)
; As with libft, loop through the pointer until we reach \0. Return count.
ft_strlen:
	mov 	rax, 0				; Initialize length counter to 0

comparison:
	cmp		BYTE [rdi + rax], 0	; compare the byte at rdi plus current count of bytes
								; 	with null character
	je		finished			; if current byte is equal to \0, we are done
								; otherwise fall through to increment

increment:
	inc		rax					; Increment the length/byte counter
	jmp		comparison			; Jump back to the comparison step to check the next byte

finished:
	ret							; Return the length of the string in rax
