    .data
msg_start:      .asciz "*** P8 Syscall Test - All New Syscalls\n"
msg_getpid:     .asciz "*** getpid: "
msg_getppid:    .asciz "*** getppid: "
msg_brk:        .asciz "*** brk(0): 0x"
msg_gettimeofday: .asciz "*** gettimeofday: sec="
msg_complete:   .asciz "*** Test Complete - All syscalls executed!\n"
newline:        .asciz "\n"
hex_digits:     .asciz "0123456789ABCDEF"

    .text
    .global start

# Helper: print string (address in %esi)
print_str:
    pushal
.Lprint_loop:
    movzbl (%esi), %eax
    test %eax, %eax
    jz .Lprint_done
    
    # write syscall (1)
    push %eax           # Save char
    mov $1, %eax        # syscall 1 = write
    mov $1, 4(%esp)     # fd = 1 (stdout) - adjust for pushed eax
    lea 4(%esp), %ecx
    mov %ecx, 8(%esp)   # buf = &char
    movl $1, 12(%esp)   # count = 1
    int $48
    add $4, %esp        # Pop char
    
    inc %esi
    jmp .Lprint_loop
.Lprint_done:
    popal
    ret

# Helper: print decimal number in %eax
print_decimal:
    pushal
    mov %eax, %ecx
    
    # Convert to ASCII and print
    push $0             # Null terminator
    mov $10, %ebx
.Lconv_loop:
    xor %edx, %edx
    div %ebx
    add $'0', %edx
    push %edx
    test %eax, %eax
    jnz .Lconv_loop
    
.Lprint_digits:
    pop %eax
    test %eax, %eax
    jz .Lprint_dec_done
    
    # Write digit
    push %eax
    mov $1, %eax
    mov $1, 4(%esp)
    lea 4(%esp), %ecx
    mov %ecx, 8(%esp)
    movl $1, 12(%esp)
    int $48
    add $4, %esp
    jmp .Lprint_digits
    
.Lprint_dec_done:
    popal
    ret

# Helper: print hex number in %eax
print_hex:
    pushal
    mov %eax, %ecx
    mov $8, %edx        # 8 hex digits
.Lhex_loop:
    rol $4, %ecx
    mov %ecx, %eax
    and $0xF, %eax
    lea hex_digits, %esi
    add %eax, %esi
    movzbl (%esi), %eax
    
    push %eax
    mov $1, %eax
    mov $1, 4(%esp)
    lea 4(%esp), %ebx
    mov %ebx, 8(%esp)
    movl $1, 12(%esp)
    int $48
    add $4, %esp
    
    dec %edx
    jnz .Lhex_loop
    popal
    ret

start:
    # Print start message
    lea msg_start, %esi
    call print_str
    
    # Test 1: getpid (syscall 20)
    lea msg_getpid, %esi
    call print_str
    mov $20, %eax
    int $48
    call print_decimal
    lea newline, %esi
    call print_str
    
    # Test 2: getppid (syscall 21)
    lea msg_getppid, %esi
    call print_str
    mov $21, %eax
    int $48
    call print_decimal
    lea newline, %esi
    call print_str
    
    # Test 3: brk(0) (syscall 31)
    lea msg_brk, %esi
    call print_str
    push $0
    mov $31, %eax
    int $48
    add $4, %esp
    call print_hex
    lea newline, %esi
    call print_str
    
    # Test 4: gettimeofday (syscall 32)
    lea msg_gettimeofday, %esi
    call print_str
    sub $8, %esp        # Space for timeval
    push $0             # tz = NULL
    push %esp
    add $4, (%esp)      # Adjust pointer
    mov $32, %eax
    int $48
    add $8, %esp
    pop %eax            # Get tv_sec
    add $4, %esp        # Skip tv_usec
    call print_decimal
    lea newline, %esi
    call print_str
    
    # Print complete message
    lea msg_complete, %esi
    call print_str
    
    # Shutdown (syscall 7)
    mov $7, %eax
    int $48
    
loop:
    jmp loop

