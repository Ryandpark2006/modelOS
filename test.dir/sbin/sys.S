	#
	# user-side system calls
	#
	# System calls use a special convention:
        #     %eax  -  system call number
        #
        #

	# void exit(int status)
	.global exit
exit:
	mov $0,%eax
	int $48
	ret

	# ssize_t write(int fd, void* buf, size_t nbyte)
	.global write
write:
	mov $1,%eax
	int $48
	ret

	# int fork()
	.global fork
fork:
	push %ebx
	push %esi
	push %edi
	push %ebp
	mov $2,%eax
	int $48
	pop %ebp
	pop %edi
	pop %esi
	pop %ebx
	ret

	# int sem(uint32_t init)
	.global sem
sem:
	mov $3,%eax
	int $48
	ret

	# int up(int s)
	.global up
up:
	mov $4,%eax
	int $48
	ret

	# int down(int s)
	.global down
down:
	mov $5,%eax
	int $48
	ret

	# int close(int id)
	.global close
close:
	mov $6,%eax
	int $48
	ret

	# int shutdown(void)
	.global shutdown
shutdown:
	mov $7,%eax
	int $48
	ret

	# int wait(int id, uint32_t *ptr)
	.global wait
wait:
	mov $8,%eax
	int $48
	ret

	# int execl(const char* path, const char* arg0, ....);
	.global execl
execl:
	mov $9,%eax
	int $48
	ret

	# int open(const char* fn)
	.global open
open:
	mov $10,%eax
	int $48
	ret


	# ssize_t len(int fd)
	.global len
len:
	mov $11,%eax
	int $48
	ret

	# ssize_t read(int fd, void* buffer, size_t n)
	.global read
read:
	mov $12,%eax
	int $48
	ret

	# off_t seek(int fd, off_t off)
	.global seek
seek:
	mov $13,%eax
	int $48
	ret

	# int chdir(const char* path)
	.global chdir
chdir:
	mov $100,%eax
	int $48
	ret

	# void* naive_mmap(uint32_t size, int is_shared, int fd, uint32_t offset)
	.global naive_mmap
naive_mmap:
	mov $101,%eax
	int $48
	ret

	# void naive_munmap(void* ptr)
	.global naive_munmap
naive_munmap:
	mov $102,%eax
	int $48
	ret

	# void sleep(uint32_t seconds)
	.global sleep
sleep:
	mov $103,%eax
	int $48
	ret

	# void iamateapot()
	.global iamateapot
iamateapot:
	mov $418,%eax
	int $48
	ret

# ========== NEW P8 SYSCALLS ==========

	# int getpid()
	.global getpid
getpid:
	pushl %ebx
	movl $20,%eax
	int $48
	popl %ebx
	ret

	# int getppid()
	.global getppid
getppid:
	pushl %ebx
	movl $64,%eax
	int $48
	popl %ebx
	ret

	# int stat(const char* path, struct stat* buf)
	.global stat
stat:
	pushl %ebx
	movl 8(%esp),%ebx   # path
	movl 12(%esp),%ecx  # buf
	movl $106,%eax
	int $48
	popl %ebx
	ret

	# int fstat(int fd, struct stat* buf)
	.global fstat
fstat:
	pushl %ebx
	movl 8(%esp),%ebx   # fd
	movl 12(%esp),%ecx  # buf
	movl $108,%eax
	int $48
	popl %ebx
	ret

	# int lstat(const char* path, struct stat* buf)
	.global lstat
lstat:
	pushl %ebx
	movl 8(%esp),%ebx   # path
	movl 12(%esp),%ecx  # buf
	movl $107,%eax
	int $48
	popl %ebx
	ret

	# int getdents(int fd, void* dirp, unsigned int count)
	.global getdents
getdents:
	pushl %ebx
	movl 8(%esp),%ebx   # fd
	movl 12(%esp),%ecx  # dirp
	movl 16(%esp),%edx  # count
	movl $141,%eax
	int $48
	popl %ebx
	ret

	# char* getcwd(char* buf, size_t size)
	.global getcwd
getcwd:
	pushl %ebx
	movl 8(%esp),%ebx   # buf
	movl 12(%esp),%ecx  # size
	movl $183,%eax
	int $48
	popl %ebx
	ret

	# int mkdir(const char* pathname, mode_t mode)
	.global mkdir
mkdir:
	pushl %ebx
	movl 8(%esp),%ebx   # pathname
	movl 12(%esp),%ecx  # mode
	movl $39,%eax
	int $48
	popl %ebx
	ret

	# int rmdir(const char* pathname)
	.global rmdir
rmdir:
	pushl %ebx
	movl 8(%esp),%ebx   # pathname
	movl $40,%eax
	int $48
	popl %ebx
	ret

	# int unlink(const char* pathname)
	.global unlink
unlink:
	pushl %ebx
	movl 8(%esp),%ebx   # pathname
	movl $10,%eax
	int $48
	popl %ebx
	ret

	# int rename(const char* oldpath, const char* newpath)
	.global rename
rename:
	pushl %ebx
	movl 8(%esp),%ebx   # oldpath
	movl 12(%esp),%ecx  # newpath
	movl $38,%eax
	int $48
	popl %ebx
	ret

	# int brk(void* addr)
	.global brk
brk:
	pushl %ebx
	movl 8(%esp),%ebx   # addr
	movl $45,%eax
	int $48
	popl %ebx
	ret

	# uint32_t time(uint32_t* tloc)
	.global time
time:
	pushl %ebx
	movl 8(%esp),%ebx   # tloc
	movl $13,%eax
	int $48
	popl %ebx
	ret

	# int gettimeofday(struct timeval* tv, struct timezone* tz)
	.global gettimeofday
gettimeofday:
	pushl %ebx
	movl 8(%esp),%ebx   # tv
	movl 12(%esp),%ecx  # tz
	movl $78,%eax
	int $48
	popl %ebx
	ret

	# int nanosleep(const struct timespec* req, struct timespec* rem)
	.global nanosleep
nanosleep:
	pushl %ebx
	movl 8(%esp),%ebx   # req
	movl 12(%esp),%ecx  # rem
	movl $162,%eax
	int $48
	popl %ebx
	ret

	# int mprotect(void* addr, size_t len, int prot)
	.global mprotect
mprotect:
	pushl %ebx
	movl 8(%esp),%ebx   # addr
	movl 12(%esp),%ecx  # len
	movl 16(%esp),%edx  # prot
	movl $125,%eax
	int $48
	popl %ebx
	ret
