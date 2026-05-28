section .text
	global ft_read
	extern __errno_location

; Registers in use:
;					rax: syscall ID, output
;					rdi: syscall FD, input argument fd
;					rsi: syscall buffer, input argument buf
;					rdx: syscall count, input argument count
;					r13: temporary register for errno handling
;					Defined by x86-64 System V ABI calling convention
; prototype: ssize_t read(int fd, void buf[.count], size_t count)
;
; The syscall is simple, all we need is already placed into the argument registers.
; The return of the syscall is stored in rax as is standard. From this we need
; 	to calculate and return either the bytes written or -1 on error.
; The syscall returns a negative value if there is an error, and from this we
; 	extract the errno and set it.

ft_read:
	mov		rax, 0				; Syscall ID for read is 0
	syscall						; Make the syscall with the arguments in rdi, rsi, rdx
								; The return value of the syscall is now in rax.

errCheck:
	cmp		rax, -4095			; Check if rax is in the error range (-1 to -4095)
								; For sys_read, a simple negative check would be OK,
								; 	but that is not common practice
	jbe		finished			; If below -4095 unsigned, it's a valid return value
								; 	return the value in rax. Otherwise, fall through
								; 	to set errno and return -1

setError:
	push	r13							; Save r13 on the stack to oblige callee-saving
	neg		rax							; Negate rax to get the positive errno value
	mov		r13, rax					; Move the errno value to r13 for later use
	call	__errno_location wrt ..plt	; Get the address of errno and store it in rax
	mov		DWORD [rax], r13d			; Set errno to the value in r13d (the lower 32 bits of r13)
	mov		rax, -1						; Set return value to -1 to indicate an error
	pop		r13							; Restore r13 from the stack

finished:
	ret							; Return the result of the syscall in rax
