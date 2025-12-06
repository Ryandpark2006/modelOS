    .data
msg1:   .asciz "*** Testing getpid (syscall 20)\n"
msg2:   .asciz "*** Testing getppid (syscall 64)\n"
msg3:   .asciz "*** Testing brk (syscall 45)\n"
msg4:   .asciz "*** Testing gettimeofday (syscall 78)\n"
msg5:   .asciz "*** Testing stat (syscall 106)\n"
msg6:   .asciz "*** Testing fstat (syscall 108)\n"
msg7:   .asciz "*** Testing getcwd (syscall 183)\n"
msg8:   .asciz "*** Testing getdents (syscall 141)\n"
msg9:   .asciz "*** Testing nanosleep (syscall 162)\n"
msg10:  .asciz "*** All P8 syscalls executed successfully!\n"

    .text
    .global start

# Simple print - string address in %ebx
print:
    push %esi
    push %edi
    
    # Calculate length
    mov %ebx, %esi
    xor %edi, %edi
.Llen:
    cmpb $0, (%esi,%edi,1)
    je .Ldone
    inc %edi
    jmp .Llen
.Ldone:
    # Write
    push %edi
    push %ebx
    push $1
    mov $1, %eax
    int $48
    add $12, %esp
    
    pop %edi
    pop %esi
    ret

start:
    # getpid (20)
    lea msg1, %ebx
    call print
    mov $20, %eax
    int $48
    
    # getppid (64)
    lea msg2, %ebx
    call print
    mov $64, %eax
    int $48
    
    # brk (45)
    lea msg3, %ebx
    call print
    push $0
    mov $45, %eax
    int $48
    add $4, %esp
    
    # gettimeofday (78)
    lea msg4, %ebx
    call print
    sub $16, %esp
    push $0
    push %esp
    addl $4, (%esp)
    mov $78, %eax
    int $48
    add $8, %esp
    add $16, %esp
    
    # stat (106)
    lea msg5, %ebx
    call print
    sub $128, %esp
    push %esp
    push $hello
    mov $106, %eax
    int $48
    add $8, %esp
    add $128, %esp
    
    # fstat (108) - fd 0 = stdin
    lea msg6, %ebx
    call print
    sub $128, %esp
    push %esp
    push $0
    mov $108, %eax
    int $48
    add $8, %esp
    add $128, %esp
    
    # getcwd (183)
    lea msg7, %ebx
    call print
    sub $256, %esp
    push $256
    push %esp
    addl $4, (%esp)
    mov $183, %eax
    int $48
    add $8, %esp
    add $256, %esp
    
    # getdents (141) - read root directory
    lea msg8, %ebx
    call print
    sub $1024, %esp
    push $1024
    push %esp
    addl $4, (%esp)
    push $0
    mov $141, %eax
    int $48
    add $12, %esp
    add $1024, %esp
    
    # nanosleep (162)
    lea msg9, %ebx
    call print
    sub $8, %esp
    movl $0, (%esp)      # tv_sec = 0
    movl $100000, 4(%esp) # tv_nsec = 100000
    push $0              # rem = NULL
    push %esp
    addl $4, (%esp)
    mov $162, %eax
    int $48
    add $8, %esp
    add $8, %esp
    
    # Success!
    lea msg10, %ebx
    call print
    
    # Shutdown (7)
    mov $7, %eax
    int $48
    
.Lhang:
    jmp .Lhang

    .section .rodata
hello: .asciz "/hello"

