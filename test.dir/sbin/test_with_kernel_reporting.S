    .text
    .global start

start:
    # Test 1: getpid - kernel will log the result
    mov $20, %eax
    int $48
    # Save result for kernel validation
    mov %eax, %ebx
    
    # Use a special syscall to report test result
    # We'll use write to fd=2 (stderr) with a special marker
    push $1             # length
    push %ebx           # the PID value
    push $2             # fd=stderr  
    mov $1, %eax        # write syscall
    int $48
    add $12, %esp
    
    # Test if PID > 0
    test %ebx, %ebx
    jle .Lfail_getpid
    
    # PASS: continue to next test
    jmp .Ltest_getppid
    
.Lfail_getpid:
    # FAIL: Infinite loop to signal failure
    jmp .Lfail_getpid

.Ltest_getppid:
    # Test 2: getppid
    mov $64, %eax
    int $48
    test %eax, %eax
    js .Lfail_getppid
    jmp .Ltest_brk
    
.Lfail_getppid:
    jmp .Lfail_getppid

.Ltest_brk:
    # Test 3: brk(0) should return != -1
    push $0
    mov $45, %eax
    int $48
    add $4, %esp
    cmp $-1, %eax
    je .Lfail_brk
    jmp .Ltest_time
    
.Lfail_brk:
    jmp .Lfail_brk

.Ltest_time:
    # Test 4: gettimeofday
    sub $16, %esp
    push $0
    push %esp
    addl $4, (%esp)
    mov $78, %eax
    int $48
    add $8, %esp
    test %eax, %eax
    jnz .Lfail_time
    pop %ebx           # tv_sec
    add $12, %esp
    test %ebx, %ebx
    jle .Lfail_time
    jmp .Ltest_stat
    
.Lfail_time:
    add $16, %esp
    jmp .Lfail_time

.Ltest_stat:
    # Test 5: stat(/hello)
    sub $128, %esp
    push %esp
    push $hello
    mov $106, %eax
    int $48
    add $8, %esp
    test %eax, %eax
    jnz .Lfail_stat
    mov 28(%esp), %ebx  # st_size
    add $128, %esp
    test %ebx, %ebx
    jle .Lfail_stat
    jmp .Ltest_fstat
    
.Lfail_stat:
    add $128, %esp
    jmp .Lfail_stat

.Ltest_fstat:
    # Test 6: fstat(0)
    sub $128, %esp
    push %esp
    push $0
    mov $108, %eax
    int $48
    add $8, %esp
    add $128, %esp
    test %eax, %eax
    jnz .Lfail_fstat
    jmp .Ltest_getcwd
    
.Lfail_fstat:
    jmp .Lfail_fstat

.Ltest_getcwd:
    # Test 7: getcwd
    sub $256, %esp
    push $256
    push %esp
    addl $4, (%esp)
    mov $183, %eax
    int $48
    add $8, %esp
    add $256, %esp
    test %eax, %eax
    jz .Lfail_getcwd
    jmp .Lsuccess
    
.Lfail_getcwd:
    jmp .Lfail_getcwd

.Lsuccess:
    # All tests passed! Exit code 42 to signal success
    push $42
    mov $0, %eax        # exit syscall
    int $48
    
hello: .asciz "/hello"

