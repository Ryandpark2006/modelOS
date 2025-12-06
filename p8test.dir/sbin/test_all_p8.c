#include "sys.h"

void print(const char* msg) {
    write(1, (void*)msg, 200);
}

void print_num(int n) {
    char buf[20];
    int i = 0;
    if (n == 0) { write(1, "0", 1); return; }
    if (n < 0) { write(1, "-", 1); n = -n; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (i > 0) { write(1, &buf[--i], 1); }
}

int main() {
    print("*** Complete P8 Syscall Test\n");
    
    // 1. getpid
    print("| getpid: ");
    pid_t pid = getpid();
    print_num(pid);
    print((pid > 0) ? " PASS\n" : " FAIL\n");
    
    // 2. getppid
    print("| getppid: ");
    pid_t ppid = getppid();
    print_num(ppid);
    print((ppid >= 0) ? " PASS\n" : " FAIL\n");
    
    // 3. brk
    print("| brk: ");
    int brk1 = (int)brk(0);
    brk((void*)(brk1 + 4096));  // Extend break
    int brk3 = (int)brk(0);
    print_num(brk3);
    print((brk3 > brk1) ? " PASS (extended)\n" : " FAIL\n");
    
    // 4. gettimeofday
    print("| gettimeofday: ");
    struct timeval tv;
    int ret = gettimeofday(&tv, 0);
    print_num(tv.tv_sec);
    print("s ");
    print_num(tv.tv_usec);
    print("us");
    print((ret == 0 && tv.tv_sec > 0) ? " PASS\n" : " FAIL\n");
    
    // 5. stat on /hello
    print("| stat(/hello): ");
    struct stat st;
    ret = stat("/hello", &st);
    print("size=");
    print_num(st.st_size);
    print(" ino=");
    print_num(st.st_ino);
    print((ret == 0 && st.st_size > 0) ? " PASS\n" : " FAIL\n");
    
    // 6. open and fstat
    print("| fstat: ");
    int fd = open("/hello", 0);
    if (fd >= 0) {
        ret = fstat(fd, &st);
        print("fd=");
        print_num(fd);
        print(" ino=");
        print_num(st.st_ino);
        print((ret == 0) ? " PASS\n" : " FAIL\n");
        close(fd);
    } else {
        print("FAIL (open failed)\n");
    }
    
    // 7. lstat
    print("| lstat(/hello): ");
    ret = lstat("/hello", &st);
    print("ino=");
    print_num(st.st_ino);
    print((ret == 0) ? " PASS\n" : " FAIL\n");
    
    // 8. getcwd
    print("| getcwd: ");
    char buf[256];
    char* cwd = getcwd(buf, sizeof(buf));
    if (cwd && buf[0] == '/') {
        print(buf);
        print(" PASS\n");
    } else {
        print("FAIL\n");
    }
    
    // 9. getdents (open /)
    print("| getdents: ");
    fd = open("/", 0);
    if (fd >= 0) {
        struct linux_dirent dirents[16];
        int count = getdents(fd, dirents, sizeof(dirents));
        print("count=");
        print_num(count);
        print((count > 0) ? " PASS\n" : " FAIL\n");
        close(fd);
    } else {
        print("FAIL (open / failed)\n");
    }
    
    // 10-14. Stub syscalls (should return -1)
    print("| mkdir (stub): ");
    ret = mkdir("/testdir", 0755);
    print((ret == -1) ? "PASS (returns -1)\n" : "FAIL\n");
    
    print("| rmdir (stub): ");
    ret = rmdir("/testdir");
    print((ret == -1) ? "PASS (returns -1)\n" : "FAIL\n");
    
    print("| unlink (stub): ");
    ret = unlink("/testfile");
    print((ret == -1) ? "PASS (returns -1)\n" : "FAIL\n");
    
    print("| rename (stub): ");
    ret = rename("/old", "/new");
    print((ret == -1) ? "PASS (returns -1)\n" : "FAIL\n");
    
    print("| mprotect (stub): ");
    ret = mprotect((void*)0x80000000, 4096, 0);
    print((ret == -1) ? "PASS (returns -1)\n" : "FAIL\n");
    
    // 15. nanosleep
    print("| nanosleep: ");
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 1000000; // 1ms
    ret = nanosleep(&req, 0);
    print((ret == 0) ? "PASS\n" : "FAIL\n");
    
    print("*** ALL 15 SYSCALLS TESTED! ***\n");
    shutdown();
    return 0;
}
