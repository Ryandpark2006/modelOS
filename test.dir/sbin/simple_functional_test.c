#include "sys.h"

// Simple test - just check if syscalls return reasonable values
// Exit with 0 if all pass, 1-7 for specific failures

void start(void) {
    // Test 1: getpid should return > 0
    pid_t pid = getpid();
    if (pid <= 0) exit(1);
    
    // Test 2: getppid should return >= 0
    pid_t ppid = getppid();
    if (ppid < 0) exit(2);
    
    // Test 3: brk(0) should return != -1
    void* brk_val = brk(0);
    if (brk_val == (void*)-1) exit(3);
    
    // Test 4: gettimeofday should return 0 and tv_sec > 0
    struct timeval tv;
    int ret = gettimeofday(&tv, 0);
    if (ret != 0 || tv.tv_sec <= 0) exit(4);
    
    // Test 5: stat("/hello") should return 0 and st_size > 0
    struct stat st;
    ret = stat("/hello", &st);
    if (ret != 0 || st.st_size <= 0 || st.st_ino <= 0) exit(5);
    
    // Test 6: fstat(0) should return 0
    ret = fstat(0, &st);
    if (ret != 0 || st.st_ino <= 0) exit(6);
    
    // Test 7: getcwd should return non-NULL and start with '/'
    char buf[256];
    char* cwd = getcwd(buf, sizeof(buf));
    if (cwd == 0 || buf[0] != '/') exit(7);
    
    // ALL TESTS PASSED!
    exit(42);
}

