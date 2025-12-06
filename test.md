# Test Case: Syscall Functionality Tests

## Author
- Balakrishna Ravalupali

## What This Test Does

This test case validates the functionality of 30+ Linux-compatible syscalls implemented in the Prog8 kernel. It exercises all major categories of system calls to ensure they work correctly.

## Syscalls Tested

### File System Operations (10 syscalls)
- `stat` (#106) - Get file metadata by path
- `fstat` (#108) - Get file metadata by file descriptor  
- `lstat` (#107) - Get file metadata without following symlinks
- `getdents` (#141) - Read directory entries
- `getcwd` (#183) - Get current working directory
- `chdir` (#100) - Change current working directory
- `mkdir` (#39) - Create directory (stub - expects failure)
- `rmdir` (#40) - Remove directory (stub - expects failure)
- `unlink` (#10) - Delete file (stub - expects failure)
- `rename` (#38) - Rename file (stub - expects failure)

### Process Management (6 syscalls)
- `getpid` (#20) - Get process ID
- `getppid` (#64) - Get parent process ID
- `fork` (#2) - Create child process
- `exit` (#0) - Terminate process
- `wait` (#8) - Wait for child process
- `execl` (#9) - Execute program

### Memory Management (3 syscalls)
- `brk` (#45) - Set program break (enables malloc)
- `mprotect` (#125) - Change memory protection (stub)
- `naive_mmap` (#101), `naive_munmap` (#102) - Memory mapping

### Time Operations (4 syscalls)
- `time` (#13) - Get time in seconds
- `gettimeofday` (#78) - Get time with microsecond precision
- `nanosleep` (#162) - Sleep with nanosecond precision
- `sleep` (#103) - Sleep for seconds

### File I/O (6 syscalls)
- `open` (#10) - Open file
- `read` (#12) - Read from file
- `write` (#1) - Write to file/console
- `close` (#6) - Close file descriptor
- `seek` (#13) - Seek in file
- `len` (#11) - Get file length (custom)

## What Each Test Verifies

1. **Process Management Tests:**
   - `getpid()` returns a valid PID (> 0)
   - `getppid()` returns parent PID (>= 0)
   - `fork()` creates child, parent waits for child exit

2. **File System Metadata Tests:**
   - `stat()` retrieves file metadata (inode, size, mode)
   - `fstat()` works on open file descriptors
   - `lstat()` handles symlinks correctly
   - `getcwd()` returns absolute path starting with '/'
   - `chdir()` changes directory and `getcwd()` reflects the change

3. **Directory Operations:**
   - `getdents()` reads directory entries with valid inode numbers and names

4. **File I/O Tests:**
   - `open()` returns valid file descriptors
   - `read()` reads data from files
   - `write()` outputs to stdout
   - `close()`, `seek()`, `len()` work correctly

5. **Memory Management:**
   - `brk(0)` returns current program break
   - `brk(addr)` extends the heap
   - `mprotect()` doesn't crash (stub implementation)

6. **Time Operations:**
   - `time()` returns seconds since boot
   - `gettimeofday()` returns valid seconds and microseconds
   - `nanosleep()` blocks for the specified time

7. **Stub Tests:**
   - `mkdir()`, `rmdir()`, `unlink()`, `rename()` fail gracefully
   - These print debug messages indicating they need Ext2 write support

## Test Structure

- **init.c** - Automated test suite that runs all tests
- **test_syscalls.c** - Comprehensive test program (same as init)
- **test_individual.c** - Individual syscall testing for debugging

## Expected Output

All 21 tests should pass, with stub syscalls correctly returning -1 and printing debug messages.

## Success Criteria

- All 21 tests pass
- No kernel panics or crashes
- Stub syscalls fail gracefully with appropriate messages
- Process, file system, memory, and time operations work correctly

## Testing Instructions

```bash
# Build the test
cd /u/rpark/cs439/Prog8
make -s clean test

# Or run the test manually
make test.test
```

## Future Work

The stub implementations (mkdir, rmdir, unlink, rename) will be fully implemented once Ext2 write support is added to the kernel. This will enable file and directory creation, deletion, and renaming operations.




