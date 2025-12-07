    .data
msg_start:    .asciz "=== FUNCTIONAL VALIDATION TEST ===\n"
msg_pass:     .asciz "PASS\n"
msg_fail:     .asciz "FAIL\n"
msg_getpid:   .asciz "getpid: "
msg_getppid:  .asciz "getppid: "
msg_brk:      .asciz "brk: "
msg_time:     .asciz "gettimeofday: "
msg_stat:     .asciz "stat: "
msg_fstat:    .asciz "fstat: "
msg_getcwd:   .asciz "getcwd: "
msg_done:     .asciz "=== ALL TESTS PASSED ===\n"

    .text
    .global start

# Print string at %esi
print:
    push %eax
    push %ebx
    push %ecx
    push %edx
    
    # Get length
    mov %esi, %ebx
    xor %ecx, %ecx
.Lloop:
    cmpb $0, (%ebx,%ecx)
    je .Ldone_len
    inc %ecx
    jmp .Lloop
.Ldone_len:
    # Write(fd=1, buf=%esi, count=%ecx)
    push %ecx
    push %esi
    push $1
    mov $1, %eax
    int $48
    add $12, %esp
    
    pop %edx
    pop %ecx
    pop %ebx
    pop %eax
    ret

start:
    # Print header
    lea msg_start, %esi
    call print
    
    # Test 1: getpid - should return > 0
    lea msg_getpid, %esi
    call print
    mov $20, %eax
    int $48
    test %eax, %eax
    jle .Lfail1
    lea msg_pass, %esi
    call print
    jmp .Ltest2
.Lfail1:
    lea msg_fail, %esi
    call print
    
.Ltest2:
    # Test 2: getppid - should return >= 0
    lea msg_getppid, %esi
    call print
    mov $64, %eax
    int $48
    test %eax, %eax
    js .Lfail2
    lea msg_pass, %esi
    call print
    jmp .Ltest3
.Lfail2:
    lea msg_fail, %esi
    call print
    
.Ltest3:
    # Test 3: brk(0) - should return current break (not -1)
    lea msg_brk, %esi
    call print
    push $0
    mov $45, %eax
    int $48
    add $4, %esp
    cmp $-1, %eax
    je .Lfail3
    lea msg_pass, %esi
    call print
    jmp .Ltest4
.Lfail3:
    lea msg_fail, %esi
    call print
    
.Ltest4:
    # Test 4: gettimeofday - should return 0 and tv_sec > 0
    lea msg_time, %esi
    call print
    sub $16, %esp
    push $0
    push %esp
    addl $4, (%esp)
    mov $78, %eax
    int $48
    add $8, %esp
    test %eax, %eax
    jnz .Lfail4
    pop %ebx           # tv_sec
    add $12, %esp
    test %ebx, %ebx
    jle .Lfail4
    lea msg_pass, %esi
    call print
    jmp .Ltest5
.Lfail4:
    add $16, %esp
    lea msg_fail, %esi
    call print
    
.Ltest5:
    # Test 5: stat(/hello) - should return 0 and st_size > 0
    lea msg_stat, %esi
    call print
    sub $128, %esp
    push %esp
    push $hello_path
    mov $106, %eax
    int $48
    add $8, %esp
    test %eax, %eax
    jnz .Lfail5
    mov 28(%esp), %ebx  # st_size at offset 28
    add $128, %esp
    test %ebx, %ebx
    jle .Lfail5
    lea msg_pass, %esi
    call print
    jmp .Ltest6
.Lfail5:
    add $128, %esp
    lea msg_fail, %esi
    call print
    
.Ltest6:
    # Test 6: fstat(0) - should return 0
    lea msg_fstat, %esi
    call print
    sub $128, %esp
    push %esp
    push $0
    mov $108, %eax
    int $48
    add $8, %esp
    add $128, %esp
    test %eax, %eax
    jnz .Lfail6
    lea msg_pass, %esi
    call print
    jmp .Ltest7
.Lfail6:
    lea msg_fail, %esi
    call print
    
.Ltest7:
    # Test 7: getcwd - should return non-zero
    lea msg_getcwd, %esi
    call print
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
    lea msg_pass, %esi
    call print
    jmp .Lsuccess
.Lfail7:
    lea msg_fail, %esi
    call print
    
.Lsuccess:
    lea msg_done, %esi
    call print
    
    # Shutdown
    mov $7, %eax
    int $48
    
.Lhang:
    jmp .Lhang

hello_path: .asciz "/hello"

