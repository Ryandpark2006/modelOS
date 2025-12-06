UTILS = init shell test_p8

CFLAGS = -std=c99 -m32 -nostdlib -g -O2 -Wall -Werror

all : $(UTILS)

OFILES = sys.o crt0.o libc.o heap.o machine.o printf.o

# keep all files
.SECONDARY :

%.o :  Makefile %.c
	gcc -c -MD $(CFLAGS) $*.c
	objcopy -R .note.gnu.property $*.o 2>/dev/null || true

%.o :  Makefile %.S
	gcc -MD -m32 -c $*.S
	objcopy -R .note.gnu.property $*.o 2>/dev/null || true

%.o :  Makefile %.s
	gcc -MD -m32 -c $*.s

$(UTILS) : % : Makefile %.o $(OFILES)
	ld -N -m elf_i386 -e start -Ttext=0x80000000 --build-id=none -o $@  $*.o $(OFILES)

clean ::
	rm -f *.o
	rm -f *.d
	#rm -f $(UTILS)

-include *.d
