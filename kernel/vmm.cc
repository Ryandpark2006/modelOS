#include "vmm.h"
#include "machine.h"
#include "idt.h"
#include "libk.h"
#include "blocking_lock.h"
#include "config.h"
#include "threads.h"
#include "debug.h"
#include "ext2.h"
#include "physmem.h"

namespace impl::VMM
{

    using namespace PhysMem;

    uint32_t *global_pd;
    impl::vme::VMES *global_vmes;
    BlockingLock *global_vmes_lock;

    uint32_t va2pa(uint32_t *pd, uint32_t va)
    {
        uint32_t pdi = (va >> 22);
        auto pde = pd[pdi];
        if (pde & 1)
        {
            auto pt = (uint32_t *)(pde & 0xFFFFF000);
            auto pti = (va >> 12) & 0x3ff;
            return pt[pti];
        }
        else
        {
            return 0;
        }
    }

    void map(uint32_t *pd, uint32_t va, uint32_t pa, uint32_t bits)
    {
        uint32_t pdi = (va >> 22);
        uint32_t pde = pd[pdi];
        if ((pde & 1) == 0)
        {
            pde = PhysMem::alloc_frame() | 7;
            bzero((void *)(pde & 0xFFFFF000), FRAME_SIZE);
            pd[pdi] = pde;
        }
        uint32_t *pt = (uint32_t *)(pde & 0xFFFFF000);
        uint32_t pti = (va >> 12) & 0x3ff;
        pt[pti] = (pa & 0xFFFFF000) | bits | 1;
        invlpg(va);
    }

    void unmap(uint32_t *pd, uint32_t va, bool dealloc)
    {
        uint32_t pdi = (va >> 22);
        uint32_t pde = pd[pdi];
        if ((pde & 1) == 0)
            return;
        uint32_t *pt = (uint32_t *)(pde & 0xFFFFF000);
        uint32_t pti = (va >> 12) & 0x3ff;
        auto pte = pt[pti];
        if ((pte & 1) == 0)
            return;
        auto pa = (0xFFFFF000 & pte);
        pt[pti] = 0;
        invlpg(va);
        if (dealloc)
        {
            dealloc_frame(pa);
        }
    }

}

using namespace impl::VMM;

namespace VMM
{

    void global_init()
    {
        using namespace impl::VMM;
        using namespace impl::vme;

        global_pd = (uint32_t *)PhysMem::alloc_frame();
        global_vmes_lock = new BlockingLock();
        auto shared_start = uint64_t(uint32_t(0xF000'0000));
        global_vmes = new VMES(shared_start, shared_start + 0x1000'0000);

        // identity mapping
        for (uint32_t va = 4096; va < kConfig.memSize; va += 4096)
        {
            map(global_pd, va, va, 3);
        }

        // pics
        map(global_pd, kConfig.localAPIC, kConfig.localAPIC, 3);
        map(global_pd, kConfig.ioAPIC, kConfig.ioAPIC, 3);

        auto inject = [](uintptr_t va_)
        {
            auto va = uint64_t(framedown(va_));
            auto vme = new VME(va, va + FRAME_SIZE, {}, 0, 0);
            auto p = global_vmes->first;
            while ((p != nullptr) && (p->next != nullptr) && (p->next->range_start <= va))
            {
                p = p->next;
            }
            vme->next = p->next;
            p->next = vme;
        };

        inject(kConfig.ioAPIC);
        inject(kConfig.localAPIC);
    }

    void per_core_init()
    {
    }

    void naive_munmap(void *p_)
    {
        using namespace PhysMem;
        using namespace impl::threads;

        auto va = framedown((uintptr_t)p_);
        if ((va < 0x80000000) || (va >= 0xF0000000))
            return;

        auto tcb = state.current();
        auto vme = tcb->vmes.remove(va);
        if (vme != nullptr)
        {
            for (uintptr_t addr = vme->range_start; addr < vme->range_end; addr += FRAME_SIZE)
            {
                unmap(tcb->pd, addr, true);
            }
            delete vme;
        }
    }

    void *naive_mmap(uint32_t sz_, bool shared, StrongPtr<Node> node, uint32_t offset_)
    {
        using namespace PhysMem;
        using namespace impl::threads;
        using namespace impl::VMM;

        if (sz_ == 0)
            return nullptr;

        auto tcb = state.current();
        if (tcb == nullptr)
        {
            return nullptr;
        }

        auto sz = frameup(sz_);
        auto offset = offset_;

        size_t file_size = 0;
        if (node != nullptr) {
            file_size = node->size_in_bytes();
        }

        void *ptr = nullptr;

        if (shared)
        {
            LockGuard _{*global_vmes_lock};
            ptr = (void *)global_vmes->insert(sz, node, offset, file_size);
        }
        else
        {
            ptr = (void *)tcb->vmes.insert(sz, node, offset, file_size);
        }

        return ptr;
    }

} /* namespace vmm */

extern "C" void vmm_pageFault(uintptr_t va_, uintptr_t *saveState)
{
    using namespace PhysMem;
    using namespace impl::threads;

    // Stack layout after pusha:
    // saveState[0-7] = EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX (from pusha)
    // saveState[8] = error code (pushed by CPU)
    // saveState[9] = EIP (pushed by CPU)
    // saveState[10] = CS (pushed by CPU)
    // saveState[11] = EFLAGS (pushed by CPU)
    Debug::printf("vmm_pageFault: va=0x%x eip=0x%x\n", va_, saveState[9]);

    auto tcb = state.current();
    ASSERT(tcb != nullptr);

    auto va = framedown(va_);

    auto map_it = [va_, va](impl::vme::VMES &vmes, uint32_t *pd, uint32_t bits)
    {
        Debug::printf("map_it: looking for VME for va=0x%x\n", va);
        auto vme = vmes.find(va);
        if (vme == nullptr)
        {
            Debug::panic("*** Page fault at unmapped address 0x%x\n", va_);
            return;
        }
        Debug::printf("map_it: found VME, mapping...\n");

        if (va2pa(pd, va) != 0)
        {
            return; // Already mapped
        }

        auto frame = alloc_frame();
        bzero((void *)frame, FRAME_SIZE);

        if (vme->file != nullptr && vme->file_size > 0)
        {
            uint32_t offset_in_vme = va - vme->range_start;
            uint32_t file_offset = vme->offset + offset_in_vme;
            uint32_t segment_file_end = vme->offset + vme->file_size;

            // Debug::printf("map_it: file_offset=%x segment_file_end=%x\n", file_offset, segment_file_end);

            if (file_offset < segment_file_end)
            {
                uint32_t bytes_to_read = FRAME_SIZE;
                uint32_t available = segment_file_end - file_offset;
                if (bytes_to_read > available)
                {
                    bytes_to_read = available;
                }

                // Debug::printf("map_it: reading %d bytes from offset %x\n", bytes_to_read, file_offset);
                int64_t result = vme->file->read_all(file_offset, bytes_to_read, (char *)frame);
                // Debug::printf("map_it: read_all returned %ld\n", result);
                if (result < 0)
                {
                    dealloc_frame(frame);
                    Debug::panic("*** File read error at offset %d\n", file_offset);
                }

                if (bytes_to_read < FRAME_SIZE)
                {
                    bzero((char *)frame + bytes_to_read, FRAME_SIZE - bytes_to_read);
                }
            }
            else
            {
                // Debug::printf("*** Beyond file data, this is BSS (zeroed)\n");
            }
        }

        map(pd, va, frame, bits);
        Debug::printf("map_it: mapped va=0x%x to frame=0x%x\n", va, frame);
    };

    if ((va >= 0x80000000) && (va < 0xF0000000))
    {
        // private mapping
        map_it(tcb->vmes, tcb->pd, 7);
        Debug::printf("vmm_pageFault: returning to eip=0x%x\n", saveState[9]);
        return;
    }

    if (va >= 0xF0000000)
    {
        LockGuard _{*global_vmes_lock};

        // (1) make sure it's mapped in the global_pd
        map_it(*global_vmes, global_pd, 7);

        // (2) share the pt
        auto pdi = va >> 22;
        auto pte = tcb->pd[pdi];
        if ((pte & 1) == 0)
        {
            tcb->pd[pdi] = global_pd[pdi];
        }

        return;
    }

    Debug::panic("*** can't handle page fault at %x\n", va_);
    while (true)
        pause();
}
