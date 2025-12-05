#include "libc.h"

int main() {
    printf("*** 1\n");
    printf("*** init - running new syscall tests\n");
    
    // Run the new syscall test program
    int child = fork();
    if (child == 0) {
        // Child process - run test
        execl("/sbin/test_new_syscalls", "test_new_syscalls", 0);
        printf("*** execl failed\n");
        exit(1);
    } else if (child > 0) {
        // Parent - wait for child
        uint32_t status = 0;
        wait(child, &status);
        printf("*** test completed with status %u\n", (unsigned)status);
    } else {
        printf("*** fork failed\n");
    }
    
    printf("*** shutting down\n");
    shutdown();
    return 0;
}

