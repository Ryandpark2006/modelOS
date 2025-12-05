#include "libc.h"

size_t strlen(const char* s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

void* sbrk(intptr_t increment) {
    if (increment == 0) {
        return (void*)brk(0);
    }
    
    void* current_brk = (void*)brk(0);
    if (current_brk == (void*)-1) {
        return (void*)-1;
    }
    
    void* new_brk = (void*)((char*)current_brk + increment);
    if (brk(new_brk) == -1) {
        return (void*)-1;
    }
    
    return current_brk;
}

int putchar(int c) {
    char t = (char)c;
    return write(1,&t,1);
}

int puts(const char* p) {
    char c;
    int count = 0;
    while ((c = *p++) != 0) {
        int n = putchar(c); 
        if (n < 0) return n;
        count ++;
    }
    putchar('\n');
    
    return count+1;
}

void cp(int from, int to) {
    while (1) {
        char buf[100];
        ssize_t n = read(from,buf,100);
        if (n == 0) break;
        if (n < 0) {
            printf("*** %s:%d read error, fd = %d\n",__FILE__,__LINE__,from);
            break;
        }
        char *ptr = buf;
        while (n > 0) {
            ssize_t m = write(to,ptr,n);
            if (m < 0) {
                printf("*** %s:%d write error, fd = %d\n",__FILE__,__LINE__,to);
                break;
            }
            n -= m;
            ptr += m;
        }
    }
}
