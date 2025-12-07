    .text
    .global start

start:
    # Test getpid (syscall 20)
    mov $20,%eax
    int $48
    # eax now contains PID
    
    # Test getppid (syscall 21)  
    mov $21,%eax
    int $48
    # eax now contains parent PID
    
    # Test brk(0) to get current break (syscall 31)
    push $0
    mov $31,%eax
    int $48
    add $4,%esp
    # eax now contains current program break
    
    # Test gettimeofday (syscall 32)
    sub $16,%esp          # allocate space for timeval
    push $0               # tz = NULL
    push %esp
    add $4,(%esp)         # adjust pointer
    mov $32,%eax
    int $48
    add $8,%esp
    add $16,%esp          # clean up timeval
    
    # Test shutdown (syscall 7) - end the test
    mov $7,%eax
    int $48
    
    # Should never reach here
loop:
    jmp loop


