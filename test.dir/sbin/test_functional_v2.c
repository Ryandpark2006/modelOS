/* Minimal functional test - checks return values */

typedef int pid_t;
typedef long off_t;
typedef unsigned int mode_t;
typedef unsigned long ino_t;
typedef unsigned long dev_t;
typedef unsigned int nlink_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef long time_t;
typedef long blksize_t;
typedef long blkcnt_t;

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
};

struct timeval {
    long tv_sec;
    long tv_usec;
};

/* Syscall wrappers - inline assembly */
static inline pid_t my_getpid(void) {
    int ret;
    asm volatile("int $48" : "=a"(ret) : "a"(20));
    return ret;
}

static inline pid_t my_getppid(void) {
    int ret;
    asm volatile("int $48" : "=a"(ret) : "a"(64));
    return ret;
}

static inline void* my_brk(void* addr) {
    void* ret;
    asm volatile("push %1; int $48; add $4, %%esp" : "=a"(ret) : "r"(addr), "a"(45));
    return ret;
}

static inline int my_gettimeofday(struct timeval* tv) {
    int ret;
    asm volatile("push $0; push %1; int $48; add $8, %%esp" 
                 : "=a"(ret) : "r"(tv), "a"(78));
    return ret;
}

static inline int my_stat(const char* path, struct stat* buf) {
    int ret;
    asm volatile("push %2; push %1; int $48; add $8, %%esp"
                 : "=a"(ret) : "r"(path), "r"(buf), "a"(106));
    return ret;
}

static inline int my_fstat(int fd, struct stat* buf) {
    int ret;
    asm volatile("push %2; push %1; int $48; add $8, %%esp"
                 : "=a"(ret) : "r"(fd), "r"(buf), "a"(108));
    return ret;
}

static inline char* my_getcwd(char* buf, unsigned long size) {
    char* ret;
    asm volatile("push %2; push %1; int $48; add $8, %%esp"
                 : "=a"(ret) : "r"(buf), "r"(size), "a"(183));
    return ret;
}

static inline void my_write(int fd, const char* buf, unsigned long count) {
    asm volatile("push %2; push %1; push %0; mov $1, %%eax; int $48; add $12, %%esp"
                 : : "r"(fd), "r"(buf), "r"(count) : "eax");
}

static inline void my_shutdown(void) {
    asm volatile("mov $7, %eax; int $48");
}

/* Simple print function */
static void print(const char* s) {
    const char* p = s;
    unsigned long len = 0;
    while (*p++) len++;
    my_write(1, s, len);
}

/* Simple number to string */
static void print_num(long n) {
    if (n == 0) {
        print("0");
        return;
    }
    
    char buf[32];
    int i = 0;
    int negative = 0;
    
    if (n < 0) {
        negative = 1;
        n = -n;
    }
    
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    
    if (negative) buf[i++] = '-';
    
    // Reverse
    for (int j = 0; j < i; j++) {
        char tmp = buf[j];
        buf[j] = buf[i-1-j];
        buf[i-1-j] = tmp;
    }
    buf[i] = '\0';
    
    print(buf);
}

void start(void) {
    print("=== P8 Syscall Functional Test ===\n\n");
    
    // Test getpid
    print("Testing getpid: ");
    pid_t pid = my_getpid();
    if (pid > 0) {
        print("PASS (pid=");
        print_num(pid);
        print(")\n");
    } else {
        print("FAIL\n");
    }
    
    // Test getppid
    print("Testing getppid: ");
    pid_t ppid = my_getppid();
    if (ppid >= 0) {
        print("PASS (ppid=");
        print_num(ppid);
        print(")\n");
    } else {
        print("FAIL\n");
    }
    
    // Test brk
    print("Testing brk: ");
    void* current_brk = my_brk(0);
    if (current_brk != (void*)-1) {
        print("PASS (brk=0x");
        // Print hex would be complex, just say pass
        print("...)\n");
    } else {
        print("FAIL\n");
    }
    
    // Test gettimeofday
    print("Testing gettimeofday: ");
    struct timeval tv;
    int ret = my_gettimeofday(&tv);
    if (ret == 0 && tv.tv_sec > 0) {
        print("PASS (sec=");
        print_num(tv.tv_sec);
        print(")\n");
    } else {
        print("FAIL\n");
    }
    
    // Test stat
    print("Testing stat(/hello): ");
    struct stat st;
    ret = my_stat("/hello", &st);
    if (ret == 0 && st.st_size > 0 && st.st_ino > 0) {
        print("PASS (size=");
        print_num(st.st_size);
        print(", ino=");
        print_num(st.st_ino);
        print(")\n");
    } else {
        print("FAIL\n");
    }
    
    // Test fstat
    print("Testing fstat(0): ");
    ret = my_fstat(0, &st);
    if (ret == 0 && st.st_ino > 0) {
        print("PASS (ino=");
        print_num(st.st_ino);
        print(")\n");
    } else {
        print("FAIL\n");
    }
    
    // Test getcwd
    print("Testing getcwd: ");
    char cwd[256];
    char* cwd_ret = my_getcwd(cwd, sizeof(cwd));
    if (cwd_ret != 0) {
        print("PASS (cwd=");
        print(cwd);
        print(")\n");
    } else {
        print("FAIL\n");
    }
    
    print("\n=== All Tests Complete ===\n");
    my_shutdown();
    
    while(1);
}

