#include "sys_p8.h"

extern int printf(const char* fmt, ...);

int main(int argc, char** argv) {
    printf("*** P8 Syscall Functional Test\n");
    
    // Test 1: getpid
    printf("| Test getpid: ");
    pid_t pid = getpid();
    if (pid > 0) {
        printf("PASS (pid=%d)\n", pid);
    } else {
        printf("FAIL (returned %d)\n", pid);
    }
    
    // Test 2: getppid
    printf("| Test getppid: ");
    pid_t ppid = getppid();
    if (ppid >= 0) {
        printf("PASS (ppid=%d)\n", ppid);
    } else {
        printf("FAIL (returned %d)\n", ppid);
    }
    
    // Test 3: brk
    printf("| Test brk: ");
    int brk1 = brk(0);
    if (brk1 != -1) {
        printf("PASS (brk=0x%x)\n", brk1);
    } else {
        printf("FAIL\n");
    }
    
    // Test 4: gettimeofday
    printf("| Test gettimeofday: ");
    struct timeval tv;
    int ret = gettimeofday(&tv, 0);
    if (ret == 0 && tv.tv_sec > 0) {
        printf("PASS (sec=%ld, usec=%ld)\n", tv.tv_sec, tv.tv_usec);
    } else {
        printf("FAIL (ret=%d)\n", ret);
    }
    
    // Test 5: stat
    printf("| Test stat(/hello): ");
    struct stat st;
    ret = stat("/hello", &st);
    if (ret == 0 && st.st_size > 0 && st.st_ino > 0) {
        printf("PASS (size=%ld, ino=%lu)\n", (long)st.st_size, (unsigned long)st.st_ino);
    } else {
        printf("FAIL (ret=%d)\n", ret);
    }
    
    // Test 6: fstat
    printf("| Test fstat(0): ");
    ret = fstat(0, &st);
    if (ret == 0 && st.st_ino > 0) {
        printf("PASS (ino=%lu)\n", (unsigned long)st.st_ino);
    } else {
        printf("FAIL (ret=%d)\n", ret);
    }
    
    // Test 7: getcwd
    printf("| Test getcwd: ");
    char buf[256];
    char* cwd = getcwd(buf, sizeof(buf));
    if (cwd != 0 && buf[0] == '/') {
        printf("PASS (cwd=%s)\n", buf);
    } else {
        printf("FAIL\n");
    }
    
    printf("*** done\n");
    shutdown();
    return 0;
}

