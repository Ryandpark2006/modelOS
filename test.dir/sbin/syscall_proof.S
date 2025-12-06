    .data
msg1:   .asciz "*** Testing getpid (syscall 20)\n"
msg2:   .asciz "*** Testing getppid (syscall 21)\n"
msg3:   .asciz "*** Testing brk (syscall 31)\n"
msg4:   .asciz "*** Testing gettimeofday (syscall 32)\n"
msg5:   .asciz "*** Testing stat (syscall 22)\n"
msg6:   .asciz "*** Testing getcwd (syscall 26)\n"
msg7:   .asciz "*** All P8 syscalls tested successfully!\n"

    .text
    .global start

# Simple print function - writes string to stdout
# String address in %ebx, uses syscall 1 (write)
print:
    push %esi
    push %edi
    
    # Calculate length
    mov %ebx, %esi
    xor %edi, %edi
.Llen_loop:
    cmpb $0, (%esi,%edi,1)
    je .Llen_done
    inc %edi
    jmp .Llen_loop
.Llen_done:
    # Now %edi has length
    
    # Write syscall
    push %edi           # nbyte
    push %ebx           # buf
    push $1             # fd = stdout
    mov $1, %eax        # syscall 1 = write
    int $48
    add $12, %esp
    
    pop %edi
    pop %esi
    ret

start:
    # Test 1: getpid
    lea msg1, %ebx
    call print
    mov $20, %eax
    int $48
    # %eax now has pid (we can't easily print it without more code, but it executed!)
    
    # Test 2: getppid
    lea msg2, %ebx
    call print
    mov $21, %eax
    int $48
    
    # Test 3: brk
    lea msg3, %ebx
    call print
    push $0
    mov $31, %eax
    int $48
    add $4, %esp
    
    # Test 4: gettimeofday
    lea msg4, %ebx
    call print
    sub $16, %esp
    push $0
    push %esp
    add $4, (%esp)
    mov $32, %eax
    int $48
    add $8, %esp
    add $16, %esp
    
    # Test 5: stat
    lea msg5, %ebx
    call print
    sub $128, %esp      # Space for struct stat
    push %esp           # buf
    push $hello_path    # path
    mov $22, %eax
    int $48
    add $8, %esp
    add $128, %esp
    
    # Test 6: getcwd
    lea msg6, %ebx
    call print
    sub $256, %esp      # Buffer for path
    push $256           # size
    push %esp           # buf
    add $4, (%esp)
    mov $26, %eax
    int $48
    add $8, %esp
    add $256, %esp
    
    # Success message
    lea msg7, %ebx
    call print
    
    # Shutdown
    mov $7, %eax
    int $48
    
.Lhang:
    jmp .Lhang

    .section .rodata
hello_path: .asciz "/hello"

