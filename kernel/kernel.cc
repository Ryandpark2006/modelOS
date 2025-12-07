#include "debug.h"
#include "ide.h"
#include "ext2.h"
#include "elf.h"
#include "machine.h"
#include "libk.h"
#include "config.h"
#include "shared.h"

extern void sysInit(StrongPtr<Ext2> fs);

void kernelMain(void) {
    auto d = StrongPtr<Ide>::make(1,0);
    auto fs = StrongPtr<Ext2>::make(d);
    
    // Initialize syscall system with filesystem
    sysInit(fs);
    
    auto init = fs->find(fs->root, "/sbin/init");
    
    if (init == nullptr) {
        Debug::panic("*** /sbin/init not found\n");
    }

    Debug::printf("loading init\n");
    uint32_t e = ELF::load(init);
    Debug::printf("entry %x\n",e);
    
    if (e == 0) {
        Debug::panic("*** ELF::load returned 0 - failed to load init\n");
    }
    
    auto userEsp = 0xF0000000 - 16; // where did this come from
    Debug::printf("user esp %x\n",userEsp);
    // Current state:
    //     - %eip points somewhere in the middle of kernelMain
    //     - %cs contains kernelCS (CPL = 0)
    //     - %esp points in the middle of the thread's kernel stack
    //     - %ss contains kernelSS
    //     - %eflags has IF=1
    // Desired state:
    //     - %eip points at e
    //     - %cs contains userCS (CPL = 3)
    //     - %eflags continues to have IF=1
    //     - %esp points to the bottom of the user stack
    //     - %ss contain userSS
    // User mode will never "return" from switchToUser. It will
    // enter the kernel through interrupts, exceptions, and system
    // calls.
    switchToUser(e,userEsp,0);
    Debug::panic("*** implement switchToUser in machine.S\n");
}

