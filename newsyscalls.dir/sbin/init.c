#include "sys.h"
#include "libc.h"

extern int printf(const char* fmt, ...);

int main() {
    printf("*** Testing new P8 syscalls\n");
    
    // Test 1: getpid/getppid
    pid_t pid = getpid();
    pid_t ppid = getppid();
    printf("*** Test 1: getpid=%d, getppid=%d\n", pid, ppid);
    
    // Test 2: getcwd
    char cwd[256];
    char* result = getcwd(cwd, sizeof(cwd));
    if (result != 0) {
        printf("*** Test 2: getcwd=%s\n", cwd);
    } else {
        printf("*** Test 2: getcwd failed\n");
    }
    
    // Test 3: stat on root directory
    struct stat st;
    int rc = stat("/", &st);
    if (rc == 0) {
        printf("*** Test 3: stat(/) size=%ld mode=%d\n", (long)st.st_size, (int)st.st_mode);
    } else {
        printf("*** Test 3: stat(/) failed\n");
    }
    
    // Test 4: fstat on stdout
    rc = fstat(1, &st);
    if (rc == 0) {
        printf("*** Test 4: fstat(1) mode=%d\n", (int)st.st_mode);
    } else {
        printf("*** Test 4: fstat(1) failed\n");
    }
    
    // Test 5: lstat (should behave like stat for now)
    rc = lstat("/", &st);
    if (rc == 0) {
        printf("*** Test 5: lstat(/) size=%ld\n", (long)st.st_size);
    } else {
        printf("*** Test 5: lstat(/) failed\n");
    }
    
    // Test 6: getdents on root directory
    int fd = open("/", 0);
    if (fd >= 0) {
        struct linux_dirent dirents[10];
        int nread = getdents(fd, dirents, sizeof(dirents));
        if (nread > 0) {
            printf("*** Test 6: getdents(/) read %d bytes\n", nread);
            // Print first entry name
            if (nread >= sizeof(struct linux_dirent)) {
                printf("*** Test 6: first entry: %s\n", dirents[0].d_name);
            }
        } else {
            printf("*** Test 6: getdents(/) failed\n");
        }
        close(fd);
    } else {
        printf("*** Test 6: open(/) failed\n");
    }
    
    // Test 7: brk/sbrk
    void* initial_brk = (void*)brk(0);
    printf("*** Test 7: initial brk=%p\n", initial_brk);
    void* new_mem = sbrk(4096);
    if (new_mem != (void*)-1) {
        printf("*** Test 7: sbrk(4096) returned %p\n", new_mem);
        void* final_brk = (void*)brk(0);
        printf("*** Test 7: final brk=%p (grew by %d)\n", final_brk, 
               (int)((uint32_t)final_brk - (uint32_t)initial_brk));
    } else {
        printf("*** Test 7: sbrk failed\n");
    }
    
    // Test 8: gettimeofday
    struct timeval tv;
    rc = gettimeofday(&tv, 0);
    if (rc == 0) {
        printf("*** Test 8: gettimeofday sec=%ld usec=%ld\n", 
               (long)tv.tv_sec, (long)tv.tv_usec);
    } else {
        printf("*** Test 8: gettimeofday failed\n");
    }
    
    // Test 9: nanosleep
    struct timespec req;
    req.tv_sec = 1;
    req.tv_nsec = 0;
    printf("*** Test 9: sleeping for 1 second...\n");
    rc = nanosleep(&req, 0);
    if (rc == 0) {
        printf("*** Test 9: nanosleep completed\n");
    } else {
        printf("*** Test 9: nanosleep failed\n");
    }
    
    // Test 10: Check time after sleep
    rc = gettimeofday(&tv, 0);
    if (rc == 0) {
        printf("*** Test 10: after sleep sec=%ld usec=%ld\n", 
               (long)tv.tv_sec, (long)tv.tv_usec);
    }
    
    // Test 11: mkdir (stub, should return -1)
    rc = mkdir("/testdir", 0755);
    printf("*** Test 11: mkdir returned %d (expected -1)\n", rc);
    
    // Test 12: rmdir (stub, should return -1)
    rc = rmdir("/testdir");
    printf("*** Test 12: rmdir returned %d (expected -1)\n", rc);
    
    // Test 13: unlink (stub, should return -1)
    rc = unlink("/testfile");
    printf("*** Test 13: unlink returned %d (expected -1)\n", rc);
    
    // Test 14: rename (stub, should return -1)
    rc = rename("/old", "/new");
    printf("*** Test 14: rename returned %d (expected -1)\n", rc);
    
    // Test 15: mprotect (stub, should return -1)
    rc = mprotect((void*)0x80000000, 4096, 0);
    printf("*** Test 15: mprotect returned %d (expected -1)\n", rc);
    
    printf("*** All tests completed\n");
    
    shutdown();
    return 0;
}

