#ifndef _SYS_H_
#define _SYS_H_

#include "stdint.h"

/****************/
/* System calls */
/****************/

typedef int ssize_t;
typedef unsigned int size_t;
typedef int pid_t;
typedef unsigned int mode_t;

/* all system calls return negative value on failure except when noted */

/* exit */
/* never returns, rc is the exit code */
extern void exit(int rc);

/* open */
/* opens a file, returns file descriptor, flags is ignored */
extern int open(const char* fn, int flags);

/* len */
/* returns number of bytes in the file, negative indicates error or a console device */
extern ssize_t len(int fd);

/* write */
/* writes up to 'nbytes' to file, returns number of bytes written */
extern ssize_t write(int fd, void* buf, size_t nbyte);

/* read */
/* reads up to nbytes from file, returns number of bytes read */
extern ssize_t read(int fd, void* buf, size_t nbyte);

/* create semaphore */
/* returns semaphore descriptor */
extern int sem(uint32_t initial);

/* up */
/* semaphore up */
/* return 0 on success, -ve value on failure */
extern int up(int id);

/* down */
/* semaphore down */
/* return 0 on success, -ve value on failure */
extern int down(int id);

/* close */
/* closes either a file or a semaphore or disowns a child process */
/* return 0 on success, -ve value on failure */
extern int close(int id);

/* shutdown */
/* should never return */
extern int shutdown(void);

/* wait */
/* wait for a child, status filled with exit value from child */
/* return 0 on success, -ve value on failure */
extern int wait(int id, uint32_t *status);

/* seek */
/* seek to given offset in file */
/* returns the new offset on success, -ve value on failure */
/* seeking in a console device is an error */
/* seeking outside the file is not an error but might cause
   subsequent read/write to fail */
extern off_t seek(int fd, off_t offset);

/* fork */
/* 0 => child, +ve => parent, -ve => error */
extern int fork();

/* execl */
/* returning indicates an error */
/* arg0 is the name of the program by convention */
/* a nullptr indicates end of arguments */
extern int execl(const char* path, const char* arg0, ...);

/* chdir */
/* changes the current working directory */
/* returns 0 on success, -1 on error */
extern int chdir(const char* path);

/* naive_mmap */
/* maps memory, possibly backed by a file */
/* returns pointer to mapped region, or negative value on error */
extern void* naive_mmap(uint32_t size, int is_shared, int fd, uint32_t offset);

/* naive_munmap */
/* unmaps a memory region */
extern void naive_munmap(void* ptr);

/* sleep */
/* sleeps for specified number of seconds */
extern void sleep(uint32_t seconds);

/* iamateapot */
/* prints a teapot message for debugging */
extern void iamateapot(void);

/* ========== NEW P8 SYSCALLS ========== */

/* getpid - get process ID */
extern int getpid(void);

/* getppid - get parent process ID */
extern int getppid(void);

/* stat - get file status by path */
struct stat {
    uint32_t st_dev;     /* Device ID */
    uint32_t st_ino;     /* Inode number */
    uint16_t st_mode;    /* File mode */
    uint16_t st_nlink;   /* Number of hard links */
    uint16_t st_uid;     /* User ID */
    uint16_t st_gid;     /* Group ID */
    uint32_t st_rdev;    /* Device ID (if special file) */
    uint32_t st_size;    /* Total size in bytes */
    uint32_t st_blksize; /* Block size for I/O */
    uint32_t st_blocks;  /* Number of blocks allocated */
    uint32_t st_atime;   /* Time of last access */
    uint32_t st_mtime;   /* Time of last modification */
    uint32_t st_ctime;   /* Time of last status change */
};

#define S_IFMT   0170000  /* File type mask */
#define S_IFREG  0100000  /* Regular file */
#define S_IFDIR  0040000  /* Directory */
#define S_IFLNK  0120000  /* Symbolic link */

extern int stat(const char* path, struct stat* buf);
extern int fstat(int fd, struct stat* buf);
extern int lstat(const char* path, struct stat* buf);

/* getdents - get directory entries */
struct linux_dirent {
    uint32_t d_ino;      /* Inode number */
    uint32_t d_off;      /* Offset to next dirent */
    uint16_t d_reclen;   /* Length of this dirent */
    char d_name[];       /* Filename (null-terminated) */
};
extern int getdents(int fd, void* dirp, unsigned int count);

/* getcwd - get current working directory */
extern char* getcwd(char* buf, size_t size);

/* mkdir - create directory */
extern int mkdir(const char* pathname, mode_t mode);

/* rmdir - remove directory */
extern int rmdir(const char* pathname);

/* unlink - delete file */
extern int unlink(const char* pathname);

/* rename - rename file or directory */
extern int rename(const char* oldpath, const char* newpath);

/* brk - set program break (for malloc) */
extern int brk(void* addr);
extern void* sbrk(intptr_t increment);

/* time operations */
extern uint32_t time(uint32_t* tloc);

struct timeval {
    uint32_t tv_sec;   /* Seconds */
    uint32_t tv_usec;  /* Microseconds */
};

struct timezone {
    int tz_minuteswest; /* Minutes west of GMT */
    int tz_dsttime;     /* Type of DST correction */
};

extern int gettimeofday(struct timeval* tv, struct timezone* tz);

struct timespec {
    uint32_t tv_sec;   /* Seconds */
    uint32_t tv_nsec;  /* Nanoseconds */
};

extern int nanosleep(const struct timespec* req, struct timespec* rem);

/* mprotect - change memory protection */
#define PROT_READ  1
#define PROT_WRITE 2
#define PROT_EXEC  4
extern int mprotect(void* addr, size_t len, int prot);

#endif
