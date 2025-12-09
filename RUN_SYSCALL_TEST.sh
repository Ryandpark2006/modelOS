#!/bin/bash
export PATH=~gheith/public/cs439/bin:$PATH
export LD_LIBRARY_PATH=~gheith/public/cs439/lib32:~gheith/public/cs439/lib64:$LD_LIBRARY_PATH

echo "Building kernel..."
make -C kernel

echo ""
echo "Running syscall test..."
echo "Look for lines like: sysHandler: syscall #20, #64, #45, #78, #106, #108, #183"
echo ""

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
  -drive file=test.data,index=1,media=disk,format=raw,file.locking=off \
  -device isa-debug-exit,iobase=0xf4,iosize=0x04

echo ""
echo "Test complete! Syscalls executed successfully if you saw lines like:"
echo "   sysHandler: syscall #20 (getpid)"
echo "   sysHandler: syscall #64 (getppid)"
echo "   sysHandler: syscall #45 (brk)"
echo "   sysHandler: syscall #78 (gettimeofday)"
echo "   sysHandler: syscall #106 (stat)"
echo "   sysHandler: syscall #108 (fstat)"
echo "   sysHandler: syscall #183 (getcwd)"
