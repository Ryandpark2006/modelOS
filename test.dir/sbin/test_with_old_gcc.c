void exit(int status);
int getpid(void);

void exit(int status) {
    asm volatile("mov $0, %eax; int $48");
}

int getpid(void) {
    int ret;
    asm volatile("mov $20, %eax; int $48" : "=a"(ret));
    return ret;
}

void start(void) {
    int pid = getpid();
    if (pid > 0) {
        exit(42);  // SUCCESS
    } else {
        exit(1);   // FAIL
    }
}
