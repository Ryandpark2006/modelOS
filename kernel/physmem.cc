#include "physmem.h"
#include "blocking_lock.h"
#include "atomic.h"
#include "debug.h"
#include "atomic.h"
#include "idt.h"
#include "smp.h"

namespace PhysMem {

    static BlockingLock *lock;

    struct Frame {
        Frame* next;
    };

    static Frame* firstFree = nullptr;
    static uint32_t avail;
    static uint32_t limit;

    uint32_t alloc_frame() {
        LockGuard g{*lock};

        uint32_t p;

        if (firstFree != nullptr) {
            p = (uint32_t) firstFree;
            firstFree = firstFree->next;
        } else {
            if (avail == limit) {
                Debug::panic("no more frames");
            }
            p = avail;
            avail += FRAME_SIZE;
        }

        ASSERT(offset(p) == 0);
        
        return p;
    }

    void dealloc_frame(uint32_t p) {
        LockGuard g{*lock};

        ASSERT(offset(p) == 0);

        Frame* f = (Frame*) p;
        f->next = firstFree;
        firstFree = f;
    }

    void dealloc_frame_atomic(uint32_t p) {
        ASSERT(offset(p) == 0);

        Frame* f = (Frame*) p;
        
        // Atomically prepend to free list
        Frame* old_first;
        do {
            old_first = firstFree;
            f->next = old_first;
        } while (!__atomic_compare_exchange_n(&firstFree, &old_first, f,
                                              false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
    }


    void init(uint32_t start, uint32_t size) {
        PhysMem::lock = new BlockingLock();
        ASSERT(offset(start) == 0);
        ASSERT(offset(size) == 0);
        Debug::printf("| physical range 0x%x 0x%x\n",start,start+size);
        avail = start;
        limit = start + size;

        /* register the page fault handler */
        IDT::trap(14,(uint32_t)pageFaultHandler_,3);
    }

};

