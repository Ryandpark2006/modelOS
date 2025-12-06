    .text
    .global start

# Syscall wrappers that follow C calling convention
exit:
    mov $0, %eax
    int $48
    ret

getpid:
    mov $20, %eax
    int $48
    ret

start:
    # Call getpid
    call getpid
    # eax now has the result
    
    # If result <= 0, exit(1)
    test %eax, %eax
    jle .Lfail
    
    # If result > 0, exit(42)
    push $42
    call exit
    
.Lfail:
    push $1
    call exit
    
.Lhang:
    jmp .Lhang

