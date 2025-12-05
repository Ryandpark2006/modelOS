#include "libc.h"

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

size_t strlen(const char* s) {
    size_t i = 0;
    while (s[i] != '\0') i++;
    return i;
}

void* sbrk(intptr_t increment) {
    void* current_break = (void*)brk(0);
    if (increment == 0) {
        return current_break;
    }
    void* new_break = (void*)brk((void*)((uint32_t)current_break + increment));
    if (new_break == (void*)-1) {
        return (void*)-1;
    }
    return current_break;
}
