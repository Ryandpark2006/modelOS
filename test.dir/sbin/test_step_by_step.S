    .text
    .global start

start:
    # Test getpid - if it fails, exit with code 1
    mov $20, %eax
    int $48
    test %eax, %eax
    jle .Lfail1
    
    # Test getppid - if it fails, exit with code 2
    mov $64, %eax
    int $48
    test %eax, %eax
    js .Lfail2
    
    # Test brk - if it fails, exit with code 3
    push $0
    mov $45, %eax
    int $48
    add $4, %esp
    cmp $-1, %eax
    je .Lfail3
    
    # Test gettimeofday - if it fails, exit with code 4
    sub $16, %esp
    push $0
    push %esp
    addl $4, (%esp)
    mov $78, %eax
    int $48
    add $8, %esp
    test %eax, %eax
    jnz .Lfail4
    pop %ebx
    add $12, %esp
    test %ebx, %ebx
    jle .Lfail4
    
    # Test stat - if it fails, exit with code 5
    sub $128, %esp
    push %esp
    push $hello
    mov $106, %eax
    int $48
    add $8, %esp
    test %eax, %eax
    jnz .Lfail5
    mov 28(%esp), %ebx
    add $128, %esp
    test %ebx, %ebx
    jle .Lfail5
    
    # Test fstat - if it fails, exit with code 6
    sub $128, %esp
    push %esp
    push $0
    mov $108, %eax
    int $48
    add $8, %esp
    add $128, %esp
    test %eax, %eax
    jnz .Lfail6
    
    # Test getcwd - if it fails, exit with code 7
    sub $256, %esp
    push $256
    push %esp
    addl $4, (%esp)
    mov $183, %eax
    int $48
    add $8, %esp
    add $256, %esp
    test %eax, %eax
    jz .Lfail7
    
    # ALL TESTS PASSED! Exit with 42
    push $42
    mov $0, %eax
    int $48

.Lfail1:
    push $1
    mov $0, %eax
    int $48
    
.Lfail2:
    push $2
    mov $0, %eax
    int $48
    
.Lfail3:
    push $3
    mov $0, %eax
    int $48
    
.Lfail4:
    push $4
    mov $0, %eax
    int $48
    
.Lfail5:
    push $5
    mov $0, %eax
    int $48
    
.Lfail6:
    push $6
    mov $0, %eax
    int $48
    
.Lfail7:
    push $7
    mov $0, %eax
    int $48

hello: .asciz "/hello"

