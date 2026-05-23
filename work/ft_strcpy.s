section .text
	global ft_strcpy

; Registers in use:
;					rax: counting
;					rdi: input argument, dest pointer
;					rsi: input argument, src pointer
; 					rcx: temporary register for byte copy (using cl)
;					Defined by x86-64 System V ABI calling convention
; Iterate through source until \0, copy to dest. Return
; Undefined if:
;				src/dest overlap
;				src is not null terminated
;				dest is not large enough to hold src (buffer overflow)
;				either argument is null or otherwise invalid
ft_strcpy:
	mov 	rax, 0				; Initialize length counter to 0

comparison:
	cmp		BYTE [rsi + rax], 0	; Compare the byte at rsi plus current count of bytes
								;	with null character
	je		finished			; If current byte is equal to \0, we are done
								;	otherwise fall through to copy and increment

copy:
	mov cl, BYTE [rsi + rax]	; Copy src byte + rax to lower byte of rcx (cl)
	mov BYTE [rdi + rax], cl	; Copy lower byte of rcx (cl)
								; This is needed because x86-64 does not allow
								; 	direct memory to memory copy, so we must use
								; 	an intermediary.

increment:
	inc		rax					; Increment the length/byte counter
	jmp		comparison			; Jump back to the comparison step to check the next byte

finished:
	mov BYTE [rdi + rax], 0		; Add null terminator to last byte
	mov rax, rdi				; Copy pointer at rsi to rax for return
	ret							; Return the pointer to dest in rax
