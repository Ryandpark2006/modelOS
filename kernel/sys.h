#ifndef _SYS_H_
#define _SYS_H_

class SYS {
public:
    static void init(void);
    
    // This is the function that is called when a user program does an "int $48"
    // The arguments are on the user stack
    static void exec(void);
};

struct iovec {
    void* iov_base;
    uint32_t iov_len;
};

#endif
