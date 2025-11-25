#pragma once

#include "blocking_lock.h"
#include "buffer.h"
#include "ide.h"
#include "block_io.h"
#include "shared.h"
#include "ide.h"
#include "queue.h"


// A simple buffer cache, flushes everything when limit is exeeded.

namespace impl::ext2 {

struct Entry {
    Entry* next = nullptr;
    StrongPtr<Buffer<char>> buffer;
    size_t block_number;

    Entry(StrongPtr<Buffer<char>> buffer, size_t const block_number): buffer(buffer), block_number(block_number) {

    }
};

class BufferCache: public BlockIO {
    BlockingLock lock{};
    StrongPtr<Ide> ide;
    uint32_t const n_buckets;
    uint32_t const limit;
    Queue<Entry, NoLock> *buckets;
    uint32_t count = 0;                // how many entries in the cache

private:
    StrongPtr<Buffer<char>> find(size_t block_number) {
        LockGuard<BlockingLock> _{lock};

        auto bucket = block_number % n_buckets;

        auto first = buckets[bucket].remove();
        if (first != nullptr) {
            auto p = first;
            do {
                if (p->block_number == block_number) {
                    buckets[bucket].add_front(p);
                    return p->buffer;
                }
                auto next = p;
                buckets[bucket].add(p);
                p = next; 
            } while (p != first);
        }
        // not in cache, see if we need to flush
        if (count == limit) {
            for (uint32_t i=0; i<n_buckets; i++) {
                auto p = buckets[i].remove_all();
                while (p != nullptr) {
                    auto next = p->next;
                    delete p;
                    p = next;
                }
            }
            count = 0;
        }

        auto buffer = StrongPtr<Buffer<char>>::make(block_size);
        ASSERT(buffer != nullptr);
        auto n = ide->read_all(block_number*block_size, block_size, (char*)(buffer->data));
        ASSERT(n == block_size);
        buckets[bucket].add_front(new Entry{buffer, block_number});
        count += 1;
        return buffer;

    }

public:
    explicit BufferCache(StrongPtr<Ide> ide, size_t block_size, size_t n_buckets, size_t limit) : BlockIO(block_size), ide(ide), n_buckets(n_buckets), limit(limit) {
        buckets = new Queue<Entry, NoLock>[n_buckets]();
    }

    virtual ~BufferCache() {
        if (buckets != nullptr) {
            delete[] buckets;
            buckets = nullptr;
        }
    }

    void read_block(uint32_t block_number, char* buffer) override {
        auto cache_buffer = find(block_number);
        ASSERT(cache_buffer != nullptr);
        ASSERT(cache_buffer->size == block_size);
        memcpy(buffer, cache_buffer->data, block_size);
    }

    uint32_t size_in_bytes() override {
        return ide->size_in_bytes();
    }

};

}