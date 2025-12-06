#include "sys.h"

// No libc, no printf - just raw syscalls
int main() {
    // Just call getpid and then shutdown
    // This should be the absolute minimum
    int pid = getpid();
    
    // Use the result so it doesn't get optimized away
    if (pid > 0) {
        shutdown();
    }
    
    return 0;
}


