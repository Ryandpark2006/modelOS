#pragma once

#include <stdint.h>
#include "shared.h"
#include "physmem.h"
#include "blocking_lock.h"

class Node;

namespace impl::VMM {
    extern uint32_t* global_pd;
    extern BlockingLock* global_vmes_lock;
    extern impl::vme::VMES* global_vmes;
    
    void map(uint32_t *pd, uint32_t va, uint32_t pa, uint32_t bits);
    void unmap(uint32_t *pd, uint32_t va, bool dealloc);
}

namespace VMM {

    // Called (on the initial core) to initialize data structures, etc
    extern void global_init();

    // Called on each core to do per-core initialization
    extern void per_core_init();

    // naive mmap
    extern void* naive_mmap(uint32_t size, bool shared, StrongPtr<Node> file, uint32_t file_offset);

    // naive munmap
    void naive_munmap(void* p);

}

