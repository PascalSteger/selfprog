[bits 32]                      ; 32 or 64-bit code

    section .data                  ; Data segment
Filename:
    db "/dev/urandom", 0x00        ; NULL-terminated path for open(...) call
DataToConvert:
    resb 1                         ; resq Reserve space for 1 quadword  (could we use resb for reserve byte?)
    dq 0xff          ; End of array terminator, corresponds to -1 0xffffffffffffffff

    section .text
;;; extern printf
;;; global main

;;; main:                           ; for gcc
global _start                   ;
_start:
;;; Open /dev/urandom
;;;     mov rax, 2                      ; open(...) is x64 syscall #2
;;;     mov rdi, Filename               ; The filename (null-terminated)
;;;     xor rsi, rsi                    ; O_RDONLY
;;;     syscall

    mov eax, 2                      ; open(...) is x64 syscall #2
    mov ebx, Filename               ; The filename (null-terminated)
    mov ecx, 0                    ; O_RDONLY
    int 80h

;;; Now populate the array with random data
;;; TODO move to 32 bit
    mov rdi, rax                    ; The file descriptor from before
    xor rax, rax                    ; read(...) is x64 syscall #0
    mov rsi, DataToConvert          ; Output to memory for conversion
    mov rdx, 1                      ; length to read, in bytes (1 quadword)
    syscall


    cmp rax, 1                      ; If we didn't get all the data...
    jne Die                         ; ...exit the program

;;; output, with gcc
;;; print string
    mov eax,4            ; The system call for write (sys_write)
    mov ebx,1                   ; File descriptor 1 - standard output
    mov ecx,DataToConvert        ; Put the offset of hello in ecx                  ;; was mov ecx,hello
    mov edx,1     ; helloLen is a constant, so we don't need to say ;; was mov edx,helloLen
    int 80h              ; Call the kernel

Die:
    ;; Call exit(EXIT_ SUCCESS)
    mov eax, 60                             ; exit() is x64 syscall #60
    xor rdi, rdi                            ; EXIT_SUCCESS = 0  written into rdi
    syscall
