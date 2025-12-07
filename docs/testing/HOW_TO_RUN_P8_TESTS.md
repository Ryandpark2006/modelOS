# How to Run P8 Syscall Functional Tests

## 📍 Test File Location

**Main Test File**: `/u/rpark/cs439/Prog8/p8test.dir/sbin/test_all_p8.c`

This file contains comprehensive functional tests for all 15 P8 syscalls.

## 🚀 Commands to Run the Tests

### Step 1: Set up the environment (REQUIRED every time)
```bash
export PATH=~gheith/public/cs439/bin:$PATH
export LD_LIBRARY_PATH=~gheith/public/cs439/lib32:~gheith/public/cs439/lib64:$LD_LIBRARY_PATH
```

### Step 2: Navigate to the test directory
```bash
cd /u/rpark/cs439/Prog8/p8test.dir/sbin
```

### Step 3: Build the test (if you made changes)
```bash
make clean
make
```

### Step 4: Deploy the test to the filesystem
```bash
cp test_all_p8 init
cd /u/rpark/cs439/Prog8
rm -f p8test.data
mkfs.ext2 -q -b 4096 -i 4096 -d p8test.dir -I 128 -r 0 -t ext2 p8test.data 10m
```

### Step 5: Run the test in QEMU
```bash
cd /u/rpark/cs439/Prog8
timeout 10 qemu-system-i386 \
  -no-reboot \
  -accel tcg,thread=multi \
  -cpu max \
  -smp 1 \
  -m 128m \
  -nographic \
  --monitor none \
  --serial stdio \
  -drive file=kernel/build/kernel.img,index=0,media=disk,format=raw,file.locking=off \
  -drive file=p8test.data,index=1,media=disk,format=raw,file.locking=off \
  -device isa-debug-exit,iobase=0xf4,iosize=0x04
```

## 🎯 One-Line Command to Build and Run

```bash
export PATH=~gheith/public/cs439/bin:$PATH && \
export LD_LIBRARY_PATH=~gheith/public/cs439/lib32:~gheith/public/cs439/lib64:$LD_LIBRARY_PATH && \
cd /u/rpark/cs439/Prog8/p8test.dir/sbin && \
make clean && make && \
cp test_all_p8 init && \
cd /u/rpark/cs439/Prog8 && \
rm -f p8test.data && \
mkfs.ext2 -q -b 4096 -i 4096 -d p8test.dir -I 128 -r 0 -t ext2 p8test.data 10m && \
timeout 10 qemu-system-i386 -no-reboot -accel tcg,thread=multi -cpu max -smp 1 -m 128m -nographic --monitor none --serial stdio -drive file=kernel/build/kernel.img,index=0,media=disk,format=raw,file.locking=off -drive file=p8test.data,index=1,media=disk,format=raw,file.locking=off -device isa-debug-exit,iobase=0xf4,iosize=0x04
```

## 📂 Important File Locations

### Test Files
- **Main test**: `/u/rpark/cs439/Prog8/p8test.dir/sbin/test_all_p8.c`
- **Simple test**: `/u/rpark/cs439/Prog8/p8test.dir/sbin/test_p8_simple.c`
- **Makefile**: `/u/rpark/cs439/Prog8/p8test.dir/sbin/Makefile`

### User-Space Syscall Infrastructure
- **Syscall wrappers (assembly)**: `/u/rpark/cs439/Prog8/p8test.dir/sbin/sys.S`
- **Syscall header**: `/u/rpark/cs439/Prog8/p8test.dir/sbin/sys.h`
- **C runtime start**: `/u/rpark/cs439/Prog8/p8test.dir/sbin/crt0_simple.S`

### Kernel Files
- **Syscall implementations**: `/u/rpark/cs439/Prog8/kernel/sys.cc`
- **Kernel image**: `/u/rpark/cs439/Prog8/kernel/build/kernel.img`

### Filesystem
- **Test filesystem**: `/u/rpark/cs439/Prog8/p8test.data` (generated)
- **Test directory**: `/u/rpark/cs439/Prog8/p8test.dir/`

## 🔧 Building Just the Kernel (if you changed kernel code)

```bash
cd /u/rpark/cs439/Prog8/kernel
make
```

## 📊 What the Test Does

The `test_all_p8.c` test validates all 15 P8 syscalls:

1. **getpid** - Checks process ID
2. **getppid** - Checks parent process ID
3. **brk** - Tests heap extension
4. **gettimeofday** - Validates time retrieval
5. **stat** - Tests file status by path
6. **fstat** - Tests file status by descriptor
7. **lstat** - Tests symbolic link status
8. **getcwd** - Tests current directory retrieval
9. **getdents** - Tests directory entry reading
10. **mkdir** - Tests directory creation (stub)
11. **rmdir** - Tests directory removal (stub)
12. **unlink** - Tests file deletion (stub)
13. **rename** - Tests file renaming (stub)
14. **mprotect** - Tests memory protection (stub)
15. **nanosleep** - Tests sleep functionality

Each test prints **PASS** or **FAIL** with relevant details.

## ⚠️ Common Issues

### Issue: "ELF::load returned 0"
**Solution**: The binary has wrong ELF format. Make sure you:
1. Use the course toolchain (Step 1)
2. Build with the provided Makefile which includes `objcopy -R .note.gnu.property`

### Issue: Tests hang or don't produce output
**Solution**: 
- Check that syscall numbers in `sys.S` match those in `kernel/sys.cc`
- Ensure you compiled with `-fcf-protection=none`

### Issue: "unknown system call" errors
**Solution**: Syscall numbers in `sys.S` don't match kernel expectations. See `COMPLETE_TEST_SUMMARY.md` for correct numbers.

## 📝 Test Output

Successful test output looks like:
```
*** Complete P8 Syscall Test
| getpid: 1 PASS
| getppid: 0 PASS
| brk: 4096 PASS (extended)
| gettimeofday: 5s 123456us PASS
| stat(/hello): size=13 ino=12 PASS
...
*** ALL 15 SYSCALLS TESTED! ***
```

## 🎓 For Graders/TAs

To quickly verify the P8 implementation works:

```bash
cd /u/rpark/cs439/Prog8
export PATH=~gheith/public/cs439/bin:$PATH
export LD_LIBRARY_PATH=~gheith/public/cs439/lib32:~gheith/public/cs439/lib64:$LD_LIBRARY_PATH
./run_p8_tests.sh  # If script exists
# OR use the one-line command above
```

The test validates that:
- ✅ All 15 syscalls are callable without crashing
- ✅ Syscalls return appropriate values
- ✅ Syscalls integrate correctly with kernel subsystems
- ✅ Stub syscalls properly return -1

