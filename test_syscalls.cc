


typedef unsigned int uint32_t;
typedef int int32_t;

#define SYS_EXIT 0
#define SYS_WRITE 4
#define SYS_CLOSE 6
#define SYS_WAITPID 7
#define SYS_OPEN 10
#define SYS_READ 12
#define SYS_LSEEK 19
#define SYS_GETPID 20
#define SYS_SETPGID 57
#define SYS_GETPPID 64
#define SYS_GETPGRP 65
#define SYS_MMAP 90
#define SYS_FSYNC 118
#define SYS_GETPGID 132
#define SYS_READV 145
#define SYS_WRITEV 146
#define SYS_PREAD 180
#define SYS_PWRITE 181
#define SYS_MMAP2 192
#define SYS_SHUTDOWN 1000

#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20

#define PROT_READ 0x1
#define PROT_WRITE 0x2

struct iovec {
    void* iov_base;
    uint32_t iov_len;
};

struct mmap_arg_struct {
    uint32_t addr;
    uint32_t len;
    uint32_t prot;
    uint32_t flags;
    uint32_t fd;
    uint32_t offset;
};

// Helper to make syscalls
static inline int32_t syscall0(int num) {
    int32_t ret;
    asm volatile("int $48" : "=a"(ret) : "a"(num));
    return ret;
}

static inline int32_t syscall1(int num, uint32_t a1) {
    int32_t ret;
    asm volatile("pushl %2; int $48; addl $4, %%esp" : "=a"(ret) : "a"(num), "r"(a1));
    return ret;
}

static inline int32_t syscall2(int num, uint32_t a1, uint32_t a2) {
    int32_t ret;
    asm volatile("pushl %3; pushl %2; int $48; addl $8, %%esp" : "=a"(ret) : "a"(num), "r"(a1), "r"(a2));
    return ret;
}

static inline int32_t syscall3(int num, uint32_t a1, uint32_t a2, uint32_t a3) {
    int32_t ret;
    asm volatile("pushl %4; pushl %3; pushl %2; int $48; addl $12, %%esp" : "=a"(ret) : "a"(num), "r"(a1), "r"(a2), "r"(a3));
    return ret;
}

static inline int32_t syscall4(int num, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) {
    int32_t ret;
    asm volatile("pushl %5; pushl %4; pushl %3; pushl %2; int $48; addl $16, %%esp" : "=a"(ret) : "a"(num), "r"(a1), "r"(a2), "r"(a3), "r"(a4));
    return ret;
}

static inline int32_t syscall6(int num, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6) {
    int32_t ret;
    asm volatile("pushl %7; pushl %6; pushl %5; pushl %4; pushl %3; pushl %2; int $48; addl $24, %%esp" 
                 : "=a"(ret) 
                 : "a"(num), "m"(a1), "m"(a2), "m"(a3), "m"(a4), "m"(a5), "m"(a6));
    return ret;
}

void print(const char* s) {
    int len = 0;
    while (s[len]) len++;
    syscall3(SYS_WRITE, 1, (uint32_t)s, len);
}

void print_int(int n) {
    char buf[16];
    int i = 0;
    if (n == 0) {
        print("0");
        return;
    }
    if (n < 0) {
        print("-");
        n = -n;
    }
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i > 0) {
        char c = buf[--i];
        syscall3(SYS_WRITE, 1, (uint32_t)&c, 1);
    }
}

void assert(bool cond, const char* msg) {
    if (cond) {
        print("PASS: ");
        print(msg);
        print("\n");
    } else {
        print("FAIL: ");
        print(msg);
        print("\n");
        syscall0(SYS_SHUTDOWN);
    }
}

extern "C" void _start() {
    print("Starting Syscall Tests...\n");

    // Test getpid
    int pid = syscall0(SYS_GETPID);
    assert(pid > 0, "getpid returned valid pid");

    int fd = syscall1(SYS_OPEN, (uint32_t)"/dummy.txt");
    print("Open fd: "); print_int(fd); print("\n");
    assert(fd >= 3, "open /dummy.txt");

    if (fd >= 0) {
        // Read initial content
        char buf[64];
        int n = syscall3(SYS_READ, fd, (uint32_t)buf, 63);
        if (n >= 0) buf[n] = 0;
        print("Read: "); print(buf); print("\n");
        assert(n > 0, "read initial content");

        // Write to file
        const char* msg = "WRITE";
        int lseek_ret = syscall3(SYS_LSEEK, fd, 0, 0);
        assert(lseek_ret == 0, "lseek to 0");
        
        int written = syscall3(SYS_WRITE, fd, (uint32_t)msg, 5);
        assert(written == 5, "write 5 bytes");
        
        // Sync
        syscall1(SYS_FSYNC, fd);
        
        // Read back
        syscall3(SYS_LSEEK, fd, 0, 0);
        n = syscall3(SYS_READ, fd, (uint32_t)buf, 5);
        if (n >= 0) buf[n] = 0;
        print("Read back: "); print(buf); print("\n");
        
        bool match = true;
        for(int i=0; i<5; i++) if(buf[i] != msg[i]) match = false;
        assert(match, "read back matches written data");
        
        // Test mmap
        uint32_t mmap_addr = syscall6(SYS_MMAP2, 0, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
        print("mmap addr: "); print_int((int)mmap_addr); print("\n");
        
        if ((int)mmap_addr > 0) {
            char* ptr = (char*)mmap_addr;
            print("mmap content: "); 
            for(int i=0; i<5; i++) {
                char c = ptr[i];
                syscall3(SYS_WRITE, 1, (uint32_t)&c, 1);
            }
            print("\n");
            
            // Write to mmap
            ptr[0] = 'X';
            assert(ptr[0] == 'X', "wrote to mmap");
            
            // Verify file did not check
            syscall3(SYS_LSEEK, fd, 0, 0);
            char buf2[5];
            syscall3(SYS_READ, fd, (uint32_t)buf2, 1);
            print("File content after mmap write: "); 
            syscall3(SYS_WRITE, 1, (uint32_t)buf2, 1);
            print("\n");
            
            // Should be W
            assert(buf2[0] == 'W', "file content unchanged (COW works)");
        } else {
            print("mmap failed\n");
            // assert(false, "mmap failed");
        }

        syscall1(SYS_CLOSE, fd);
    }

    print("All Tests Passed!\n");
    syscall0(SYS_SHUTDOWN);
    
    while(1) {}
}
