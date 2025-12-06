    .text
    .global start

start:
    # Test getpid
    mov $20, %eax
    int $48
    # eax now has the result
    
    # If result <= 0, exit with 1
    test %eax, %eax
    jle .Lfail
    
    # If result > 0, exit with 42 (success!)
    push $42
    mov $0, %eax
    int $48
    
.Lfail:
    push $1
    mov $0, %eax
    int $48
    
.Lhang:
    jmp .Lhang

