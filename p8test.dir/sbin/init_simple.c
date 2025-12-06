#include "sys.h"
#include "libc.h"

extern int printf(const char* fmt, ...);

int main() {
    printf("*** P8 Simple Test Starting\n");
    
    // Test 1: getpid
    pid_t pid = getpid();
    printf("*** getpid returned: %d\n", pid);
    
    // Test 2: getppid
    pid_t ppid = getppid();
    printf("*** getppid returned: %d\n", ppid);
    
    // Test 3: brk
    void* brk_result = (void*)brk(0);
    printf("*** brk(0) returned: 0x%x\n", (unsigned int)brk_result);
    
    printf("*** P8 Simple Test Complete\n");
    shutdown();
    return 0;
}

