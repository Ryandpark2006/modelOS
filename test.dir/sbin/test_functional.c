#include "sys.h"
#include "libc.h"

void test_getpid() {
    puts("Testing getpid...");
    pid_t pid = getpid();
    if (pid > 0) {
        puts("  ✓ getpid returned positive PID");
    } else {
        puts("  ✗ getpid failed");
    }
}

void test_getppid() {
    puts("Testing getppid...");
    pid_t ppid = getppid();
    if (ppid >= 0) {
        puts("  ✓ getppid returned valid PPID");
    } else {
        puts("  ✗ getppid failed");
    }
}

void test_brk() {
    puts("Testing brk...");
    void* current_brk = sbrk(0);
    if (current_brk != (void*)-1) {
        puts("  ✓ brk(0) returned current break");
        
        // Try to extend by 4KB
        void* new_brk = sbrk(4096);
        if (new_brk != (void*)-1) {
            puts("  ✓ sbrk(4096) extended heap");
        } else {
            puts("  ✗ sbrk(4096) failed");
        }
    } else {
        puts("  ✗ brk failed");
    }
}

void test_gettimeofday() {
    puts("Testing gettimeofday...");
    struct timeval tv;
    int ret = gettimeofday(&tv, 0);
    if (ret == 0 && tv.tv_sec > 0) {
        puts("  ✓ gettimeofday returned time");
    } else {
        puts("  ✗ gettimeofday failed");
    }
}

void test_stat() {
    puts("Testing stat...");
    struct stat st;
    int ret = stat("/hello", &st);
    if (ret == 0) {
        puts("  ✓ stat(/hello) succeeded");
        if (st.st_size > 0) {
            puts("  ✓ stat returned file size > 0");
        }
        if (st.st_ino > 0) {
            puts("  ✓ stat returned inode number > 0");
        }
    } else {
        puts("  ✗ stat failed");
    }
}

void test_fstat() {
    puts("Testing fstat...");
    struct stat st;
    int ret = fstat(0, &st);  // stdin
    if (ret == 0) {
        puts("  ✓ fstat(0) succeeded");
        if (st.st_ino > 0) {
            puts("  ✓ fstat returned inode number > 0");
        }
    } else {
        puts("  ✗ fstat failed");
    }
}

void test_getcwd() {
    puts("Testing getcwd...");
    char buf[256];
    char* ret = getcwd(buf, sizeof(buf));
    if (ret != 0) {
        puts("  ✓ getcwd succeeded: ");
        puts(buf);
    } else {
        puts("  ✗ getcwd failed");
    }
}

void start() {
    puts("═══════════════════════════════════════");
    puts("  P8 SYSCALL FUNCTIONAL TEST");
    puts("═══════════════════════════════════════\n");
    
    test_getpid();
    test_getppid();
    test_brk();
    test_gettimeofday();
    test_stat();
    test_fstat();
    test_getcwd();
    
    puts("\n═══════════════════════════════════════");
    puts("  ALL TESTS COMPLETE");
    puts("═══════════════════════════════════════");
    
    shutdown();
}

