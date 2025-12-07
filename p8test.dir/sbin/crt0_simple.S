.global start
start:
    call main
    push %eax
loop:
    call exit
    jmp loop
