# Test Plan for Syscall Functionality

## Purpose

These test programs verify **barebones functionality** of your implemented syscalls. They're simpler and faster than BusyBox.

## What's the Difference?

### Test Cases (What You Need Now) ✅
- **Purpose:** Verify each syscall works correctly
- **Complexity:** Simple (50-200 lines per test)
- **Time:** Quick to write (1-2 days)
- **Goal:** Catch bugs, verify basic functionality

### BusyBox (Optional, Later)
- **Purpose:** Comprehensive real-world validation
- **Complexity:** Complex (full utilities)
- **Time:** Longer to integrate (1-2 weeks)
- **Goal:** Prove your OS can run real programs

**Answer: Yes, test cases check barebones functionality. BusyBox would be for comprehensive validation.**

---

## Test Programs Provided

### 1. `test_syscalls.c` - Comprehensive Test Suite

Automated test suite that checks all syscalls:

**Features:**
- Tests 20+ syscalls automatically
- Pass/fail reporting
- Organized by category
- Runs in ~5 seconds

**Categories tested:**
- Process Management (getpid, getppid, fork, wait)
- File System Metadata (stat, fstat, lstat, getcwd, chdir)
- Directory Operations (getdents)
- File I/O (open, read, write, close, seek, len)
- Memory Management (brk, mprotect)
- Time Operations (time, gettimeofday, nanosleep)
- Stub Syscalls (mkdir, rmdir, unlink, rename)

**Usage:**
```bash
# Compile
cd Prog8
make test_syscalls

# Run
./test_syscalls

# Expected output:
# ========================================
#   SYSCALL FUNCTIONALITY TEST SUITE
# ========================================
# 
# --- Process Management ---
# [TEST] getpid... PASS
# [TEST] getppid... PASS
# ...
# 
# ========================================
#   TEST SUMMARY
# ========================================
# Total tests: 20
# Passed: 20
# Failed: 0
# 
# ✓ ALL TESTS PASSED!
```

### 2. `test_individual.c` - Quick Individual Tests

Test one syscall at a time for debugging:

**Usage:**
```bash
# Compile
make test_individual

# Test one syscall
./test_individual getpid
# Output: getpid() = 2

./test_individual stat
# Output:
# stat('/') returned: 0
#   inode: 2
#   size: 1024 bytes
#   mode: 0x4000
#   links: 2

./test_individual getdents
# Output:
# getdents() read 512 bytes
# Directory entries:
#   - . (inode: 2)
#   - .. (inode: 2)
#   - sbin (inode: 12)
#   - test.txt (inode: 15)

# Run all tests
./test_individual all
```

---

## How to Use These Tests

### Step 1: Add to Your Build System

Add to your `Prog8/Makefile`:

```makefile
# Test programs
test_syscalls: test_syscalls.o $(USER_OFILES)
	ld -N -m elf_i386 -e start -Ttext=0x80000000 -o $@ $^

test_individual: test_individual.o $(USER_OFILES)
	ld -N -m elf_i386 -e start -Ttext=0x80000000 -o $@ $^

test_syscalls.o: test_syscalls.c
	gcc -m32 -nostdlib -c -o $@ $<

test_individual.o: test_individual.c
	gcc -m32 -nostdlib -c -o $@ $<
```

### Step 2: Create Test Disk Image

You need a disk image with your test programs:

```bash
# Create test directory
mkdir -p test.dir/sbin

# Copy test programs
cp test_syscalls test.dir/sbin/
cp test_individual test.dir/sbin/

# Create disk image
make test.img
```

### Step 3: Run Tests

```bash
# Boot with test disk
qemu-system-i386 -kernel build/kernel.bin -drive file=test.img,format=raw

# In your OS, run:
/sbin/test_syscalls
# or
/sbin/test_individual stat
```

---

## What Each Test Checks

### ✅ Process Management Tests

1. **test_getpid()**
   - Verifies: PID is > 0
   - Expected: Returns valid process ID

2. **test_getppid()**
   - Verifies: PPID is >= 0
   - Expected: Returns parent's PID (or 0 if no parent)

3. **test_fork_simple()**
   - Verifies: Fork creates child, child exits with correct code
   - Expected: Parent gets child's exit status (42)

### ✅ File System Metadata Tests

4. **test_stat()**
   - Verifies: stat() returns file metadata
   - Expected: Inode > 0, size > 0 for root directory

5. **test_fstat()**
   - Verifies: fstat() works on open file descriptor
   - Expected: Same as stat but via fd

6. **test_lstat()**
   - Verifies: lstat() returns metadata without following symlinks
   - Expected: Same as stat for non-symlinks

7. **test_getcwd()**
   - Verifies: Returns current working directory
   - Expected: Absolute path starting with '/'

8. **test_chdir()**
   - Verifies: Changes directory and getcwd reflects change
   - Expected: Path changes after chdir

### ✅ Directory Operations Tests

9. **test_getdents()**
   - Verifies: Reads directory entries
   - Expected: At least one entry with valid inode and name

### ✅ File I/O Tests

10. **test_open_read()**
    - Verifies: Can open file and read data
    - Expected: fd >= 0, read returns >= 0 bytes

11. **test_write()**
    - Verifies: write() outputs to stdout
    - Expected: Returns number of bytes written

12. **test_file_operations()**
    - Verifies: open/read/write/close/seek work together
    - Expected: All operations succeed

### ✅ Memory Management Tests

13. **test_brk()**
    - Verifies: Can get and set program break
    - Expected: New break >= old break

14. **test_mprotect()**
    - Verifies: Doesn't crash (stub)
    - Expected: Returns 0 (success)

### ✅ Time Operations Tests

15. **test_time()**
    - Verifies: Returns time in seconds
    - Expected: >= 0

16. **test_gettimeofday()**
    - Verifies: Returns time with microsecond precision
    - Expected: seconds >= 0, microseconds < 1000000

17. **test_nanosleep()**
    - Verifies: Sleeps for specified time
    - Expected: Time advances after sleep

### ⚠️ Stub Tests (Should Fail)

18. **test_mkdir_stub()**
    - Verifies: Returns -1 (not implemented)
    - Expected: Fails gracefully

19-21. Similar for rmdir, unlink, rename

---

## Interpreting Results

### All Tests Pass ✓
```
✓ ALL TESTS PASSED!
Total tests: 20
Passed: 20
Failed: 0
```
**Meaning:** Your syscalls are working correctly!

### Some Tests Fail ✗
```
✗ SOME TESTS FAILED
Total tests: 20
Passed: 17
Failed: 3

[TEST] stat... FAIL: stat failed
```
**Action:** 
1. Check which syscall failed
2. Add debug prints to that syscall
3. Check return values and parameters

### Specific Failures

**"open failed"**
- Check: Does the file exist?
- Check: Is your filesystem mounted?
- Check: Is open() syscall returning valid fd?

**"stat failed"**
- Check: Can you find the file?
- Check: Is the stat buffer being filled correctly?

**"getdents failed"**
- Check: Is the directory node valid?
- Check: Are you reading directory blocks correctly?
- Check: Are you formatting linux_dirent correctly?

---

## Debugging Tips

### Add Debug Prints

In `sys.cc`, add:
```cpp
case 106: // stat
{
    Debug::printf("stat called on: %s\n", path);
    // ... rest of implementation
    Debug::printf("stat returning: %d\n", ret);
    return ret;
}
```

### Check Values

```cpp
// In test
printf("fd = %d\n", fd);
printf("bytes read = %d\n", n);
printf("inode = %u\n", st.st_ino);
```

### Test One at a Time

```bash
# Instead of full suite:
./test_individual getpid  # Start with simplest
./test_individual stat    # Then more complex
```

---

## Next Steps After Tests Pass

### 1. All Tests Pass? Great! ✅
   - Your syscalls are working
   - Ready to implement Ext2 write support
   - Can start writing utilities (ls, cat, etc.)

### 2. Implement Missing Syscalls
   - Add lseek, dup, dup2 (see REMAINING_SYSCALLS.md)
   - Test each one individually

### 3. Write Simple Utilities
   - `ls` - list directory
   - `cat` - print file
   - `pwd` - print working directory

### 4. (Optional) Try BusyBox
   - After everything works well
   - As comprehensive validation

---

## Summary

### For Barebones Functionality Testing: ✅
- Use `test_syscalls.c` - automated test suite
- Use `test_individual.c` - debug individual syscalls
- **Time:** 1-2 days to write and run
- **Result:** Know which syscalls work/fail

### For Comprehensive Validation: (Later)
- Use BusyBox utilities
- **Time:** 1-2 weeks to integrate
- **Result:** Prove OS can run real programs

**You only need the test cases for now. BusyBox is optional for later comprehensive testing.**






