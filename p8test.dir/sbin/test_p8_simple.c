#include "sys.h"

// Simple printf without malloc
void simple_print(const char* msg) {
    write(1, (void*)msg, 100);  // Just write up to 100 chars
}

void simple_print_num(int n) {
    char buf[20];
    int i = 0;
    if (n == 0) {
        write(1, "0", 1);
        return;
    }
    if (n < 0) {
        write(1, "-", 1);
        n = -n;
    }
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i > 0) {
        write(1, &buf[--i], 1);
    }
}

int main(int argc, char** argv) {
    simple_print("*** P8 Syscall Test\n");
    
    // Test getpid
    simple_print("| getpid: ");
    pid_t pid = getpid();
    simple_print_num(pid);
    simple_print("\n");
    
    // Test getppid
    simple_print("| getppid: ");
    pid_t ppid = getppid();
    simple_print_num(ppid);
    simple_print("\n");
    
    // Test brk
    simple_print("| brk: ");
    int brk_val = brk(0);
    simple_print_num(brk_val);
    simple_print("\n");
    
    simple_print("*** done\n");
    shutdown();
    return 0;
}
