
// Comprehensive test for Write, Fork, and mmap (MAP_PRIVATE)
// Compiled with: g++ -m32 -static -nostdlib -fno-builtin -fno-rtti -fno-exceptions -o init test_comprehensive.cc

typedef unsigned int uint32_t;
typedef int int32_t;

// Syscall numbers
#define SYS_EXIT 0
#define SYS_WRITE 4
#define SYS_CLOSE 6
#define SYS_WAITPID 7
#define SYS_OPEN 10
#define SYS_READ 12
#define SYS_LSEEK 19
#define SYS_GETPID 20
#define SYS_FORK 2
#define SYS_MMAP 90
#define SYS_FSYNC 118
#define SYS_MMAP2 192
#define SYS_SHUTDOWN 1000

#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20

#define PROT_READ 0x1
#define PROT_WRITE 0x2

#define O_RDWR 2

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
        print("*** PASS: ");
        print(msg);
        print("\n");
    } else {
        print("*** FAIL: ");
        print(msg);
        print("\n");
        syscall0(SYS_SHUTDOWN);
    }
}

extern "C" void _start() {
    print("*** Starting Comprehensive Test...\n");

    // 1. Open file
    int fd = syscall1(SYS_OPEN, (uint32_t)"/dummy.txt");
    assert(fd >= 3, "open /dummy.txt");

    // 2. Write to file
    const char* parent_msg = "PARENT_DATA";
    syscall3(SYS_LSEEK, fd, 0, 0);
    int written = syscall3(SYS_WRITE, fd, (uint32_t)parent_msg, 11);
    assert(written == 11, "Parent wrote to file");
    syscall1(SYS_FSYNC, fd);

    // 3. Fork
    int pid = syscall0(SYS_FORK);
    
    if (pid == 0) {
        // CHILD
        print("*** Child process started\n");
        
        // 4. Map file MAP_PRIVATE
        uint32_t mmap_addr = syscall6(SYS_MMAP2, 0, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
        
        if ((int32_t)mmap_addr == -1 || mmap_addr == 0) {
            print("*** MMAP FAILED: val=");
            print_int((int)mmap_addr);
            print("\n");
            syscall0(SYS_SHUTDOWN);
        }
        
        char* ptr = (char*)mmap_addr;
        
        // Verify content
        bool match = true;
        for(int i=0; i<11; i++) if(ptr[i] != parent_msg[i]) match = false;
        
        if (!match) {
             print("*** MMAP CONTENT MISMATCH: Expected PARENT_DATA, got ");
             syscall3(SYS_WRITE, 1, (uint32_t)ptr, 11);
             print("\n");
        }
        assert(match, "Child sees parent data in mmap");
        
        // Modify memory (COW)
        ptr[0] = 'C';
        ptr[1] = 'H';
        ptr[2] = 'I';
        ptr[3] = 'L';
        ptr[4] = 'D';
        
        assert(ptr[0] == 'C', "Child modified memory");
        
        // Verify file content did NOT change
        char buf[16];
        syscall3(SYS_LSEEK, fd, 0, 0);
        syscall3(SYS_READ, fd, (uint32_t)buf, 11);
        
        bool file_match = true;
        for(int i=0; i<11; i++) if(buf[i] != parent_msg[i]) file_match = false;
        assert(file_match, "File content unchanged by child memory write (MAP_PRIVATE works)");
        
        print("*** Child exiting\n");
        syscall1(SYS_EXIT, 0);
    } else {
        // PARENT
        print("*** Parent waiting for child...\n");
        syscall3(SYS_WAITPID, pid, 0, 0);
        print("*** Child finished.\n");
        
        // Verify file content again
        char buf[16];
        syscall3(SYS_LSEEK, fd, 0, 0);
        syscall3(SYS_READ, fd, (uint32_t)buf, 11);
        
        bool file_match = true;
        for(int i=0; i<11; i++) if(buf[i] != parent_msg[i]) file_match = false;
        assert(file_match, "Parent sees original file content");
        
        syscall1(SYS_CLOSE, fd);
    }

    print("*** Comprehensive Test Passed!\n");
    syscall0(SYS_SHUTDOWN);
    
    while(1) {}
}
