#!/bin/bash
# Quick script to run P8 syscall tests

echo "Setting up environment..."
export PATH=~gheith/public/cs439/bin:$PATH
export LD_LIBRARY_PATH=~gheith/public/cs439/lib32:~gheith/public/cs439/lib64:$LD_LIBRARY_PATH

echo "Building test..."
cd /u/rpark/cs439/Prog8/p8test.dir/sbin
make clean
make

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "Deploying to filesystem..."
cp test_all_p8 init
cd /u/rpark/cs439/Prog8
rm -f p8test.data
mkfs.ext2 -q -b 4096 -i 4096 -d p8test.dir -I 128 -r 0 -t ext2 p8test.data 10m

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "🎯 Running P8 Syscall Functional Tests"
echo "═══════════════════════════════════════════════════════════"
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
  -drive file=p8test.data,index=1,media=disk,format=raw,file.locking=off \
  -device isa-debug-exit,iobase=0xf4,iosize=0x04 2>&1 | strings | grep -E "(\*\*\*|PASS|FAIL)" | head -30

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "Test complete!"
echo "═══════════════════════════════════════════════════════════"
