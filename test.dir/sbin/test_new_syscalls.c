#include "libc.h"
#include "sys.h"

void test_getpid() {
    printf("*** Testing getpid/getppid\n");
    int pid = getpid();
    int ppid = getppid();
    printf("*** PID: %d, Parent PID: %d\n", pid, ppid);
}

void test_getcwd() {
    printf("*** Testing getcwd\n");
    char cwd[256];
    char* result = getcwd(cwd, sizeof(cwd));
    if (result != 0) {
        printf("*** Current directory: %s\n", cwd);
    } else {
        printf("*** getcwd failed\n");
    }
}

void test_stat() {
    printf("*** Testing stat\n");
    struct stat st;
    int ret = stat("/sbin/init", &st);
    if (ret == 0) {
        printf("*** /sbin/init: size=%u, mode=0x%x, inode=%u\n", 
               (unsigned)st.st_size, (unsigned)st.st_mode, (unsigned)st.st_ino);
    } else {
        printf("*** stat failed\n");
    }
}

void test_fstat() {
    printf("*** Testing fstat\n");
    int fd = open("/sbin/shell", 0);
    if (fd >= 0) {
        struct stat st;
        int ret = fstat(fd, &st);
        if (ret == 0) {
            printf("*** /sbin/shell: size=%u, mode=0x%x\n", (unsigned)st.st_size, (unsigned)st.st_mode);
        } else {
            printf("*** fstat failed\n");
        }
        close(fd);
    } else {
        printf("*** open failed\n");
    }
}

void test_getdents() {
    printf("*** Testing getdents\n");
    int fd = open("/sbin", 0);
    if (fd >= 0) {
        char buffer[1024];
        int bytes = getdents(fd, buffer, sizeof(buffer));
        if (bytes > 0) {
            printf("*** Read %d bytes of directory entries\n", bytes);
            int offset = 0;
            int count = 0;
            while (offset < bytes && count < 5) {
                struct linux_dirent* d = (struct linux_dirent*)(buffer + offset);
                printf("***   %s (inode=%u)\n", d->d_name, (unsigned)d->d_ino);
                offset += d->d_reclen;
                count++;
            }
        } else {
            printf("*** getdents failed\n");
        }
        close(fd);
    } else {
        printf("*** open /sbin failed\n");
    }
}

void test_brk() {
    printf("*** Testing brk/sbrk\n");
    void* old_brk = sbrk(0);
    printf("*** Current break: %p\n", old_brk);
    
    void* new_mem = sbrk(4096);
    printf("*** Allocated 4096 bytes at: %p\n", new_mem);
    
    void* current_brk = sbrk(0);
    printf("*** New break: %p\n", current_brk);
}

void test_gettimeofday() {
    printf("*** Testing gettimeofday\n");
    struct timeval tv;
    int ret = gettimeofday(&tv, 0);
    if (ret == 0) {
        printf("*** Time: %u seconds, %u microseconds\n", (unsigned)tv.tv_sec, (unsigned)tv.tv_usec);
    } else {
        printf("*** gettimeofday failed\n");
    }
}

void test_chdir() {
    printf("*** Testing chdir\n");
    char cwd1[256];
    getcwd(cwd1, sizeof(cwd1));
    printf("*** Before chdir: %s\n", cwd1);
    
    int ret = chdir("/sbin");
    if (ret == 0) {
        char cwd2[256];
        getcwd(cwd2, sizeof(cwd2));
        printf("*** After chdir: %s\n", cwd2);
        
        // Change back
        chdir("/");
    } else {
        printf("*** chdir failed\n");
    }
}

int main() {
    printf("*** Starting new syscall tests\n");
    printf("*** \n");
    
    test_getpid();
    printf("*** \n");
    
    test_getcwd();
    printf("*** \n");
    
    test_chdir();
    printf("*** \n");
    
    test_stat();
    printf("*** \n");
    
    test_fstat();
    printf("*** \n");
    
    test_getdents();
    printf("*** \n");
    
    test_brk();
    printf("*** \n");
    
    test_gettimeofday();
    printf("*** \n");
    
    printf("*** All new syscall tests completed!\n");
    
    return 0;
}

