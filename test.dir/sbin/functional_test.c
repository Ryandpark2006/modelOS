#include "sys.h"
#include "libc.h"

int test_passed = 0;
int test_failed = 0;

void test_getpid(void) {
    puts("Test getpid: ");
    pid_t pid = getpid();
    if (pid > 0) {
        puts("PASS\n");
        test_passed++;
    } else {
        puts("FAIL\n");
        test_failed++;
    }
}

void test_getppid(void) {
    puts("Test getppid: ");
    pid_t ppid = getppid();
    if (ppid >= 0) {
        puts("PASS\n");
        test_passed++;
    } else {
        puts("FAIL\n");
        test_failed++;
    }
}

void test_brk(void) {
    puts("Test brk: ");
    void* brk1 = sbrk(0);
    void* brk2 = sbrk(4096);
    void* brk3 = sbrk(0);
    
    if (brk1 != (void*)-1 && brk2 != (void*)-1 && brk3 > brk1) {
        puts("PASS\n");
        test_passed++;
    } else {
        puts("FAIL\n");
        test_failed++;
    }
}

void test_gettimeofday(void) {
    puts("Test gettimeofday: ");
    struct timeval tv;
    int ret = gettimeofday(&tv, 0);
    if (ret == 0 && tv.tv_sec > 0) {
        puts("PASS\n");
        test_passed++;
    } else {
        puts("FAIL\n");
        test_failed++;
    }
}

void test_stat(void) {
    puts("Test stat: ");
    struct stat st;
    int ret = stat("/hello", &st);
    if (ret == 0 && st.st_size > 0 && st.st_ino > 0) {
        puts("PASS\n");
        test_passed++;
    } else {
        puts("FAIL\n");
        test_failed++;
    }
}

void test_fstat(void) {
    puts("Test fstat: ");
    struct stat st;
    int ret = fstat(0, &st);
    if (ret == 0 && st.st_ino > 0) {
        puts("PASS\n");
        test_passed++;
    } else {
        puts("FAIL\n");
        test_failed++;
    }
}

void test_getcwd(void) {
    puts("Test getcwd: ");
    char buf[256];
    char* ret = getcwd(buf, sizeof(buf));
    if (ret != 0 && buf[0] == '/') {
        puts("PASS\n");
        test_passed++;
    } else {
        puts("FAIL\n");
        test_failed++;
    }
}

void start(void) {
    puts("=== P8 SYSCALL FUNCTIONAL TEST ===\n\n");
    
    test_getpid();
    test_getppid();
    test_brk();
    test_gettimeofday();
    test_stat();
    test_fstat();
    test_getcwd();
    
    puts("\n=== RESULTS ===\n");
    puts("Passed: ");
    // TODO: print number
    puts("\nFailed: ");
    // TODO: print number
    puts("\n");
    
    if (test_failed == 0) {
        puts("ALL TESTS PASSED!\n");
        exit(0);
    } else {
        puts("SOME TESTS FAILED!\n");
        exit(1);
    }
}

