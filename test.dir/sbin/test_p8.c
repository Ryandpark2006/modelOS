#include "sys.h"
#include "libc.h"

extern int printf(const char* fmt, ...);

int main() {
    printf("*** P8 New Syscall Test\n");
    
    // Test getpid
    pid_t pid = getpid();
    printf("*** getpid: %d\n", pid);
    
    // Test getppid
    pid_t ppid = getppid();
    printf("*** getppid: %d\n", ppid);
    
    // Test brk
    void* initial_brk = (void*)brk(0);
    printf("*** brk(0): 0x%x\n", (unsigned int)initial_brk);
    
    // Test gettimeofday
    struct timeval tv;
    int rc = gettimeofday(&tv, 0);
    printf("*** gettimeofday: rc=%d, sec=%ld\n", rc, (long)tv.tv_sec);
    
    // Test stat
    struct stat st;
    rc = stat("/hello", &st);
    printf("*** stat(/hello): rc=%d, size=%ld\n", rc, (long)st.st_size);
    
    // Test getcwd
    char cwd[256];
    char* result = getcwd(cwd, sizeof(cwd));
    if (result) {
        printf("*** getcwd: %s\n", cwd);
    }
    
    printf("*** All P8 syscalls tested!\n");
    shutdown();
    return 0;
}

