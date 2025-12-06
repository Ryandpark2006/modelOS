#include "sys.h"
#include "libc.h"

extern int printf(const char* fmt, ...);

int main() {
    printf("*** P8 Syscall Test Starting\n");
    
    // Test 1: getpid/getppid
    pid_t pid = getpid();
    pid_t ppid = getppid();
    printf("*** Test 1: getpid=%d, getppid=%d\n", pid, ppid);
    
    // Test 2: stat on /hello file
    struct stat st;
    int rc = stat("/hello", &st);
    if (rc == 0) {
        printf("*** Test 2: stat(/hello) success - size=%ld, mode=0x%x\n", 
               (long)st.st_size, st.st_mode);
    } else {
        printf("*** Test 2: stat(/hello) failed with rc=%d\n", rc);
    }
    
    // Test 3: fstat on open file
    int fd = open("/hello", 0);
    if (fd >= 0) {
        struct stat fst;
        rc = fstat(fd, &fst);
        if (rc == 0) {
            printf("*** Test 3: fstat(fd=%d) success - size=%ld\n", 
                   fd, (long)fst.st_size);
        } else {
            printf("*** Test 3: fstat failed with rc=%d\n", rc);
        }
        close(fd);
    } else {
        printf("*** Test 3: open failed\n");
    }
    
    // Test 4: getcwd
    char cwd[256];
    char* result = getcwd(cwd, sizeof(cwd));
    if (result != 0) {
        printf("*** Test 4: getcwd=%s\n", cwd);
    } else {
        printf("*** Test 4: getcwd failed\n");
    }
    
    // Test 5: chdir and getcwd
    rc = chdir("/etc");
    if (rc == 0) {
        result = getcwd(cwd, sizeof(cwd));
        if (result != 0) {
            printf("*** Test 5: chdir(/etc) success, new cwd=%s\n", cwd);
        } else {
            printf("*** Test 5: chdir success but getcwd failed\n");
        }
    } else {
        printf("*** Test 5: chdir failed with rc=%d\n", rc);
    }
    
    // Test 6: brk/sbrk
    void* initial_brk = (void*)brk(0);
    void* sbrk_result = sbrk(4096);
    void* new_brk = (void*)brk(0);
    printf("*** Test 6: initial_brk=0x%x, sbrk_result=0x%x, new_brk=0x%x\n",
           (unsigned int)initial_brk, (unsigned int)sbrk_result, (unsigned int)new_brk);
    
    // Test 7: gettimeofday
    struct timeval tv;
    rc = gettimeofday(&tv, 0);
    if (rc == 0) {
        printf("*** Test 7: gettimeofday success - sec=%ld, usec=%ld\n",
               (long)tv.tv_sec, (long)tv.tv_usec);
    } else {
        printf("*** Test 7: gettimeofday failed with rc=%d\n", rc);
    }
    
    // Test 8: nanosleep
    struct timespec req;
    req.tv_sec = 1;
    req.tv_nsec = 0;
    printf("*** Test 8: sleeping for 1 second...\n");
    rc = nanosleep(&req, 0);
    printf("*** Test 8: nanosleep returned rc=%d\n", rc);
    
    // Test 9: getdents (read root directory)
    chdir("/");
    fd = open("/", 0);
    if (fd >= 0) {
        char buffer[1024];
        struct linux_dirent* d;
        int nread = getdents(fd, (struct linux_dirent*)buffer, sizeof(buffer));
        if (nread > 0) {
            printf("*** Test 9: getdents(/) returned %d bytes\n", nread);
            int pos = 0;
            int count = 0;
            while (pos < nread) {
                d = (struct linux_dirent*)(buffer + pos);
                if (count < 5) {  // Print first 5 entries
                    printf("***   Entry: %s (ino=%ld)\n", d->d_name, (long)d->d_ino);
                }
                pos += d->d_reclen;
                count++;
            }
            printf("***   Total entries: %d\n", count);
        } else {
            printf("*** Test 9: getdents failed with rc=%d\n", nread);
        }
        close(fd);
    } else {
        printf("*** Test 9: open(/) failed\n");
    }
    
    printf("*** P8 Syscall Test Complete\n");
    shutdown();
    return 0;
}

