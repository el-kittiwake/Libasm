section .text
	global ft_strcmp

; Registers in use:
;					rax: counting and output
;					rdi: input argument s1
;					rsi: input argument s2
;					rcx: temporary register for byte comparison (using cl)
;					Defined by x86-64 System V ABI calling convention
; Iterate through both strings until we find a mismatch or reach the end of both.
; Return the difference (s1 - s2). If there is no difference return 0
; Same as libft, with the same undefined behaviour:
;				s1/s2 overlap
;				s1/s2 are not null terminated
;				either argument is null or otherwise invalid
ft_strcmp:
	mov		rax, 0					; Initialise length counter to 0

prepareBytes:
	mov		cl, BYTE [rsi + rax]	; Copy s2 byte + rax to lower byte of rcx (cl)

comparison:
	cmp		cl, BYTE [rdi + rax]	; Compare the current bytes of s1 and s2
	jne		subtract				; If bytes are not equal, we are done, move to end
									; 	Otherwise, fall through to continue comparing
	cmp		BYTE [rdi + rax], 0		; Compare the current byte at rdi with null character
	je		subtract				; If current byte is equal to \0, we are done, move to end
									; 	otherwise fall through to increment

increment:
	inc		rax						; Increment the length/byte counter
	jmp		prepareBytes			; Jump back to the prepareBytes step to check the next byte

subtract:
	movzx	eax, BYTE [rdi + rax]	; Move with zero extension the byte at rdi
	movzx	ecx, cl					; Move with zero extension the byte in cl to ecx
									; Copy the smaller value to the larger register
									; 	and fill the upper bits with zeros.
	sub		eax, ecx				; Subtract the byte in ecx from the byte at eax
									; result is in rax

finished:
	ret								; Return the difference in rax
