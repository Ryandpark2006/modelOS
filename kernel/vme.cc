#include "vme.h"
#include "debug.h"
#include "ext2.h"
#include "physmem.h"
#include "config.h"

namespace impl::vme {

    /* All ranges are inclusive_start ... exclusive_end */

    VME::VME(uint64_t start, uint64_t end, StrongPtr<Node> file, size_t offset, size_t file_size):
        range_start(start), range_end(end), file(file), offset(offset), file_size(file_size)
    {
        ASSERT(end >= start);
        static_assert(sizeof(uint64_t) > sizeof(size_t));
        static_assert(sizeof(uint64_t) > sizeof(uintptr_t));
    }



    VMES::VMES(uint64_t start, uint64_t end): first(nullptr), zone_start(start), zone_end(end) {
        static_assert(sizeof(uint64_t) > sizeof(uintptr_t));
        static_assert(sizeof(uint64_t) > sizeof(size_t));

        ASSERT(end >= start);

        auto header = new VME(start, start, {}, 0, 0);
        auto footer = new VME(end, end, {}, 0, 0);
        footer->next = nullptr;
        header->next = footer;
        first = header;
    }

    VMES::~VMES() {
        auto p = first;

        while (p != nullptr) {
            auto next = p->next;
            delete p;
            p = next;
        }
    }

    void VMES::sanity() {
        auto p = first;
        while (p != nullptr) {
            auto next = p->next;
            ASSERT(p->range_end <= zone_end);
            ASSERT(p->range_start >= zone_start);
            p = next;
        }
    }

    uintptr_t VMES::insert(size_t size_, StrongPtr<Node> file, size_t offset, size_t file_size) {
        using namespace PhysMem;

        ASSERT(size_ > 0);

        auto size = uint64_t(frameup(size_));

        auto p = first;
        while ((p != nullptr) && (p->next != nullptr)) {
            auto next = p->next;
            auto hole = next->range_start - p->range_end;
                
            if (hole >= size) {
                auto va = p->range_end;

                auto n = new VME{va, va+size, file, offset, file_size};
                n->next = p->next;
                p->next = n;
                return va;
            }
            p = next;
        }
        
        return 0;
    }

    VME* VMES::remove(uintptr_t va) {
        using namespace PhysMem;

        auto p = first;
        auto pprev = &first;
        while ((p != nullptr) && (p->range_start <= va)){
            if (va < p->range_end) {
                *pprev = p->next;
                return p;
            }
            pprev = &p->next;
            p = p->next;
        }
        return nullptr;
    }

    VME* VMES::find(uintptr_t va) {
        auto p = first;
        while ((p != nullptr) && (va >= p->range_start)) {
            if (va < p->range_end) return p;
            p = p->next;
        }
        return nullptr;
    }


}