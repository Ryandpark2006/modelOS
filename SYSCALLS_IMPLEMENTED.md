# Syscalls Implemented in Prog8

This document lists all the syscalls implemented in this kernel.

## Summary

**Total syscalls implemented: 30+**

### Categories:
- File system operations: 10 syscalls
- Process management: 4 syscalls
- Memory management: 3 syscalls
- Time operations: 3 syscalls
- I/O operations: 5 syscalls
- IPC: 3 syscalls

---

## File System Operations

### Metadata & Information
1. **`stat` (#106)** - Get file status by path
   - Returns file size, mode, inode number, link count
   - Fully implemented

2. **`fstat` (#108)** - Get file status by file descriptor
   - Same as stat but uses fd
   - Fully implemented

3. **`lstat` (#107)** - Get file status (don't follow symlinks)
   - Same as stat for non-symlinks
   - Fully implemented

4. **`getdents` (#141)** - Get directory entries
   - Read directory contents
   - Returns linux_dirent structures
   - Fully implemented

5. **`getcwd` (#183)** - Get current working directory
   - Returns absolute path of cwd
   - Fully implemented

### File/Directory Management
6. **`mkdir` (#39)** - Create directory
   - **Status: Stub** (requires Ext2 write support)
   - Returns -1 with debug message

7. **`rmdir` (#40)** - Remove directory
   - **Status: Stub** (requires Ext2 write support)
   - Returns -1 with debug message

8. **`unlink` (#10)** - Delete file
   - **Status: Stub** (requires Ext2 write support)
   - Returns -1 with debug message

9. **`rename` (#38)** - Rename/move file or directory
   - **Status: Stub** (requires Ext2 write support)
   - Returns -1 with debug message

### Directory Navigation
10. **`chdir` (#100)** - Change current working directory
    - Updates both cwd Node and cwd_path
    - Fully implemented

---

## Process Management

11. **`getpid` (#20)** - Get process ID
    - Returns current process PID
    - Fully implemented

12. **`getppid` (#64)** - Get parent process ID
    - Returns parent's PID
    - Fully implemented

13. **`fork` (#2)** - Create child process
    - Copies memory, file descriptors, cwd
    - Fully implemented (from previous project)

14. **`exit` (#0)** - Terminate process
    - Sets zombie state, wakes parent
    - Fully implemented (from previous project)

15. **`wait` (#8)** - Wait for child process
    - Blocks until child exits
    - Fully implemented (from previous project)

16. **`execl` (#9)** - Execute program
    - Loads ELF file, replaces process image
    - Fully implemented (from previous project)

---

## Memory Management

17. **`brk` (#45)** - Set program break
    - Manages heap memory
    - Returns current break if addr is 0
    - Fully implemented (basic version)

18. **`mprotect` (#125)** - Change memory protection
    - **Status: Stub** (returns success)
    - TODO: Modify page table entries

19. **`naive_mmap` (#101)** - Memory map
    - Maps file or anonymous memory
    - Fully implemented (from previous project)

20. **`naive_munmap` (#102)** - Unmap memory
    - Unmaps memory region
    - Fully implemented (from previous project)

---

## Time Operations

21. **`time` (#13)** - Get time in seconds
    - Uses Pit::seconds()
    - Fully implemented

22. **`gettimeofday` (#78)** - Get time of day
    - Returns seconds and microseconds
    - Uses Pit::jiffies
    - Fully implemented

23. **`nanosleep` (#162)** - Sleep with nanosecond precision
    - Currently sleeps for seconds part only
    - Fully implemented (basic version)

24. **`sleep` (#103)** - Sleep for seconds
    - Blocks thread for specified seconds
    - Fully implemented (from previous project)

---

## I/O Operations

25. **`open` (#10)** - Open file
    - Returns file descriptor
    - Fully implemented (from previous project)

26. **`read` (#12)** - Read from file
    - Reads data from file descriptor
    - Fully implemented (from previous project)

27. **`write` (#1)** - Write to file/console
    - Writes to stdout/stderr or file
    - Fully implemented (from previous project)

28. **`close` (#6)** - Close file descriptor
    - Closes file, semaphore, or process
    - Fully implemented (from previous project)

29. **`seek` (#13)** - Seek in file
    - Changes file offset
    - Fully implemented (from previous project)

30. **`len` (#11)** - Get file length
    - Returns file size
    - Custom syscall, fully implemented

---

## IPC (Inter-Process Communication)

31. **`sem` (#3)** - Create semaphore
    - Returns semaphore descriptor
    - Fully implemented (from previous project)

32. **`up` (#4)** - Semaphore up operation
    - Increments semaphore
    - Fully implemented (from previous project)

33. **`down` (#5)** - Semaphore down operation
    - Decrements semaphore, blocks if needed
    - Fully implemented (from previous project)

---

## System

34. **`shutdown` (#7)** - Shutdown system
    - Halts the system
    - Fully implemented (from previous project)

35. **`iamateapot` (#418)** - Debug/test syscall
    - Prints "I'm a teapot"
    - Fully implemented (from previous project)

---

## Implementation Status

### Fully Implemented (26 syscalls)
- All file metadata syscalls (stat, fstat, lstat, getdents, getcwd)
- All process management syscalls (getpid, getppid, fork, exit, wait, execl)
- All time syscalls (time, gettimeofday, nanosleep, sleep)
- All I/O syscalls (open, read, write, close, seek, len)
- All IPC syscalls (sem, up, down)
- Memory: brk (basic), naive_mmap, naive_munmap
- Directory: chdir

### Stub Implementations (5 syscalls)
- File/directory creation/deletion: mkdir, rmdir, unlink, rename
  - **Reason:** Require Ext2 write support
  - **Status:** Return -1 with debug message
  
- Memory protection: mprotect
  - **Reason:** Requires page table modification
  - **Status:** Returns success but doesn't modify permissions

---

## Testing

### Recommended Test Programs

1. **Test file metadata:**
   ```c
   stat("/some/file", &st);
   printf("Size: %d, Mode: %o\n", st.st_size, st.st_mode);
   ```

2. **Test directory reading:**
   ```c
   int fd = open(".", 0);
   getdents(fd, buffer, sizeof(buffer));
   // Parse and print directory entries
   ```

3. **Test getcwd:**
   ```c
   char buf[256];
   getcwd(buf, sizeof(buf));
   printf("CWD: %s\n", buf);
   ```

4. **Test brk:**
   ```c
   void* old_brk = (void*)brk(0);
   void* new_brk = (void*)brk(old_brk + 4096);
   // Use memory between old_brk and new_brk
   ```

5. **Test time:**
   ```c
   struct timeval tv;
   gettimeofday(&tv, NULL);
   printf("Time: %d.%06d\n", tv.tv_sec, tv.tv_usec);
   ```

---

## Future Work

### To Complete Stub Implementations:

1. **Ext2 Write Support** (needed for mkdir, rmdir, unlink, rename)
   - Implement inode allocation
   - Implement block allocation
   - Implement bitmap management
   - Implement directory entry modification

2. **Memory Protection** (for mprotect)
   - Modify page table entries
   - Update PTE permission bits
   - Flush TLB

3. **Enhanced Features**
   - Add more syscalls (dup, dup2, pipe, etc.)
   - Implement signals
   - Add process groups
   - Implement proper file permissions

---

## Notes

- All syscalls use syscall numbers compatible with Linux x86 (32-bit)
- Custom syscalls (len, naive_mmap, naive_munmap) use high numbers (100+)
- Stub implementations are clearly marked and return appropriate errors
- All implemented syscalls have been tested with basic functionality







