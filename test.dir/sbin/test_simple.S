    .text
    .global start

start:
    # Test syscalls in order - kernel logging will show them
    
    # getpid (20)
    mov $20, %eax
    int $48
    
    # getppid (64)
    mov $64, %eax
    int $48
    
    # brk (45)
    push $0
    mov $45, %eax
    int $48
    add $4, %esp
    
    # gettimeofday (78)
    sub $16, %esp
    push $0
    push %esp
    addl $4, (%esp)
    mov $78, %eax
    int $48
    add $8, %esp
    add $16, %esp
    
    # stat (106)
    sub $128, %esp
    push %esp
    push $hello
    mov $106, %eax
    int $48
    add $8, %esp
    add $128, %esp
    
    # fstat (108)
    sub $128, %esp
    push %esp
    push $0
    mov $108, %eax
    int $48
    add $8, %esp
    add $128, %esp
    
    # getcwd (183)
    sub $256, %esp
    push $256
    push %esp
    addl $4, (%esp)
    mov $183, %eax
    int $48
    add $8, %esp
    add $256, %esp
    
    # shutdown (7)
    mov $7, %eax
    int $48
    
loop:
    jmp loop

    .section .rodata
hello: .asciz "/hello"

