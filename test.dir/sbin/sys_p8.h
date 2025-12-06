#ifndef _SYS_H_
#define _SYS_H_

#include "stdint.h"

/****************/
/* System calls */
/****************/

typedef int ssize_t;
typedef unsigned int size_t;

/* all system calls return negative value on failure except when noted */

/* exit */
/* never returns, rc is the exit code */
extern void exit(int rc);

/* open */
/* opens a file, returns file descriptor, flags is ignored */
extern int open(const char *fn, int flags);

/* len */
/* returns number of bytes in the file, negative indicates error or a console device */
extern ssize_t len(int fd);

/* write */
/* writes up to 'nbytes' to file, returns number of bytes written */
extern ssize_t write(int fd, void *buf, size_t nbyte);

/* read */
/* reads up to nbytes from file, returns number of bytes read */
extern ssize_t read(int fd, void *buf, size_t nbyte);

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
extern int execl(const char *path, const char *arg0, ...);

extern int chdir(const char *path);

extern int sleep(uint32_t seconds);

/* P8 new syscalls */
typedef int pid_t;
typedef unsigned int mode_t;

struct stat {
    uint32_t st_dev;
    uint32_t st_ino;
    uint16_t st_mode;
    uint16_t st_nlink;
    uint16_t st_uid;
    uint16_t st_gid;
    uint32_t st_rdev;
    uint32_t st_size;
    uint32_t st_blksize;
    uint32_t st_blocks;
    uint32_t st_atime;
    uint32_t st_mtime;
    uint32_t st_ctime;
};

struct linux_dirent {
    uint32_t d_ino;
    uint32_t d_off;
    uint16_t d_reclen;
    char d_name[256];
};

struct timeval {
    uint32_t tv_sec;
    uint32_t tv_usec;
};

struct timespec {
    uint32_t tv_sec;
    uint32_t tv_nsec;
};

extern pid_t getpid(void);
extern pid_t getppid(void);
extern int stat(const char* path, struct stat* buf);
extern int fstat(int fd, struct stat* buf);
extern int lstat(const char* path, struct stat* buf);
extern int getdents(int fd, struct linux_dirent* dirp, unsigned int count);
extern char* getcwd(char* buf, size_t size);
extern int mkdir(const char* path, mode_t mode);
extern int rmdir(const char* path);
extern int unlink(const char* path);
extern int rename(const char* oldpath, const char* newpath);
extern int brk(void* addr);
extern int gettimeofday(struct timeval* tv, void* tz);
extern int nanosleep(const struct timespec* req, struct timespec* rem);
extern int mprotect(void* addr, size_t len, int prot);

#endif
