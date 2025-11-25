/*
    Note that this is a LRU cache with hand-over-hand locking where I took inspiration from the 
    LRU cache leetcode problem as well as the lectures from class where Dr. Geith was describing this implementation. 
*/

#pragma once

#include "blocking_lock.h"
#include "shared.h"

struct CacheEntry {
    uint32_t block_number;       
    char* buffer;               
    bool valid;                  
    BlockingLock lock;           
    CacheEntry* next;            
    CacheEntry* prev;            
    
    CacheEntry() : block_number(0), buffer(nullptr), valid(false), next(nullptr), prev(nullptr) {}
};

class BlockCache {
private:
    CacheEntry* head;            
    CacheEntry* tail;            
    BlockingLock list_lock;      
    uint32_t block_size;
    uint32_t max_entries;
    uint32_t current_entries;
    
    void move_to_front(CacheEntry* entry) {
        if (entry == head) return;  
        
        if (entry->prev) entry->prev->next = entry->next;
        if (entry->next) entry->next->prev = entry->prev;
        if (entry == tail) tail = entry->prev;
        
        entry->next = head;
        entry->prev = nullptr;
        if (head) head->prev = entry;
        head = entry;
        if (!tail) tail = entry;
    }
    
    // Helper: Find entry with hand-over-hand locking
    // Returns entry with its lock HELD (caller must unlock)
    CacheEntry* find_and_lock(uint32_t block_number) {
        list_lock.lock();
        
        if (!head) {
            list_lock.unlock();
            return nullptr;
        }
        
        CacheEntry* current = head;
        current->lock.lock();
        list_lock.unlock();
        
        while (current) {
            if (current->valid && current->block_number == block_number) {
                return current;
            }
            
            CacheEntry* next = current->next;
            if (next) {
                next->lock.lock();  
            }
            current->lock.unlock();
            current = next;
        }
        
        return nullptr;  
    }
    
    CacheEntry* allocate_entry() {
        list_lock.lock();
        
        if (current_entries < max_entries) {
            CacheEntry* entry = new CacheEntry();
            entry->buffer = new char[block_size];
            current_entries++;
            
            // Add to front
            entry->next = head;
            entry->prev = nullptr;
            if (head) head->prev = entry;
            head = entry;
            if (!tail) tail = entry;
            
            list_lock.unlock();
            return entry;
        }
        
        CacheEntry* victim = tail;
        if (!victim) {
            list_lock.unlock();
            return nullptr;
        }
        
        victim->lock.lock();
        list_lock.unlock();
        
        return victim;
    }

public:
    BlockCache(uint32_t block_size, uint32_t max_entries = 32) 
        : head(nullptr), tail(nullptr), block_size(block_size), 
          max_entries(max_entries), current_entries(0) {}
    
    ~BlockCache() {
        list_lock.lock();
        CacheEntry* current = head;
        while (current) {
            CacheEntry* next = current->next;
            delete[] current->buffer;
            delete current;
            current = next;
        }
        list_lock.unlock();
    }
    
    bool read(uint32_t block_number, char* buffer) {
        list_lock.lock();
        
        CacheEntry* current = head;
        while (current) {
            if (current->valid && current->block_number == block_number) {
                for (uint32_t i = 0; i < block_size; i++) {
                    buffer[i] = current->buffer[i];
                }
                move_to_front(current);
                list_lock.unlock();
                return true;
            }
            current = current->next;
        }
        
        list_lock.unlock();
        return false;  
    }
    
    void write(uint32_t block_number, const char* buffer) {
        list_lock.lock();
        
        // Check if already in cache
        CacheEntry* current = head;
        while (current) {
            if (current->valid && current->block_number == block_number) {
                for (uint32_t i = 0; i < block_size; i++) {
                    current->buffer[i] = buffer[i];
                }
                move_to_front(current);
                list_lock.unlock();
                return;
            }
            current = current->next;
        }
        
        CacheEntry* entry;
        if (current_entries < max_entries) {
            entry = new CacheEntry();
            entry->buffer = new char[block_size];
            current_entries++;
            entry->next = head;
            entry->prev = nullptr;
            if (head) head->prev = entry;
            head = entry;
            if (!tail) tail = entry;
        } else {
            entry = tail;
        }
        
        entry->block_number = block_number;
        entry->valid = true;
        for (uint32_t i = 0; i < block_size; i++) {
            entry->buffer[i] = buffer[i];
        }
        
        list_lock.unlock();
    }
    
    void invalidate(uint32_t block_number) {
        list_lock.lock();
        CacheEntry* current = head;
        while (current) {
            if (current->valid && current->block_number == block_number) {
                current->valid = false;
                break;
            }
            current = current->next;
        }
        list_lock.unlock();
    }
};

