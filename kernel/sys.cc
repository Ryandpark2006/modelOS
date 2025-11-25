#include <stdint.h>

#include "sys.h"
#include "stdint.h"
#include "debug.h"
#include "idt.h"
#include "machine.h"
#include "ext2.h"
#include "ide.h"
#include "shared.h"
#include "threads.h"
#include "vmm.h"
#include "physmem.h"
#include "libk.h"
#include "elf.h"
#include "semaphore.h"
#include "atomic.h"

#define MAX_FDS 256
struct FileDescriptor {
    StrongPtr<Node> node;
    uint32_t offset;
    int refcount;
    bool in_use;
};

static FileDescriptor fd_table[MAX_FDS];
static StrongPtr<Ext2> global_fs;

#define MAX_SEMS 256
struct SemaphoreDescriptor {
    StrongPtr<Semaphore> sem;
    int refcount; 
    bool in_use;
};

static SemaphoreDescriptor sem_table[MAX_SEMS];

static int next_pid = 2;
static SpinLock exit_wait_lock;  

enum ProcessState {
    PROC_RUNNING,
    PROC_ZOMBIE
};

class UserProcessTCB;

#define MAX_PROCESSES 256
static UserProcessTCB* pid_to_process[MAX_PROCESSES];

class UserProcessTCB : public impl::threads::TCB {
public:
    int pid;
    uintptr_t stack[STACK_BYTES / sizeof(uintptr_t)];
    impl::threads::TCB* parent_thread;
    UserProcessTCB* first_child;
    UserProcessTCB* next_sibling;
    ProcessState state;
    int exit_status;
    UserProcessTCB* waiting_parent;
    StrongPtr<Node> cwd;  
    bool has_parent_list;
    
    UserProcessTCB(uint32_t* pd, int pid) 
        : TCB(pd), pid(pid), parent_thread(nullptr), first_child(nullptr), 
          next_sibling(nullptr), state(PROC_RUNNING), exit_status(0), 
          waiting_parent(nullptr), cwd(nullptr), has_parent_list(false) {}
    
    void doit() override {
        Debug::panic("UserProcessTCB::doit() called");
    }
    
    uint32_t interruptEsp() override {
        return uint32_t(&stack[(STACK_BYTES / sizeof(uintptr_t)) - 1]);
    }
};

static inline uint32_t get_arg(uint32_t *frame, int n) {
    uint32_t *user_stack = (uint32_t*)(frame[3]); 
    return user_stack[1 + n];
}

extern "C" int sysHandler(uint32_t eax, uint32_t *frame) {
    
    switch (eax) {
    case 0: /* exit */
    {
        using namespace impl::threads;
        
        int rc = (int)get_arg(frame, 0);
        auto current_tcb = state.current();
        
        if (current_tcb == nullptr) {
            Debug::shutdown();
            return 0;
        }
        
        auto current = static_cast<UserProcessTCB*>(current_tcb);
        if (current->parent_thread == nullptr) {
            state.block("exit", [] {
            });
            return 0;
        }
                
        exit_wait_lock.lock();
        
        current->exit_status = rc;
        current->state = PROC_ZOMBIE;
        
        UserProcessTCB* parent_to_wake = current->waiting_parent;
        current->waiting_parent = nullptr;
        
        exit_wait_lock.unlock();
        
        if (parent_to_wake != nullptr) {
            state.ready_queue.add(parent_to_wake);
        }
        
        state.block("exit", [current] {
        });
        
        // should never reach here
        return 0;
    }
    
    case 1:  // write 
    {
        int fd = (int)get_arg(frame, 0);
        void* buf = (void*)get_arg(frame, 1);
        uint32_t nbyte = get_arg(frame, 2);
        
        if (fd == 1 || fd == 2) {
            char* str = (char*)buf;
            for (uint32_t i = 0; i < nbyte; i++) {
                Debug::printf("%c", str[i]);
            }
            return nbyte;
        }
        
        if (fd >= 0 && fd < MAX_FDS && fd_table[fd].in_use) {
            auto& fde = fd_table[fd];
            uint32_t file_size = fde.node->size_in_bytes();
            
            if (fde.offset > file_size) {
                return -1;
            }
            
            fde.offset += nbyte;
            return nbyte;
        }
        
        return -1;
    }
    
    case 2: // fork 
    {
        using namespace impl::threads;
        using namespace impl::vme;
        using namespace PhysMem;
                
        auto parent_tcb = state.current();
        
        int child_pid = -1;
        for (int i = next_pid; i < MAX_PROCESSES; i++) {
            if (pid_to_process[i] == nullptr) {
                child_pid = i;
                next_pid = i + 1;
                break;
            }
        }
        if (child_pid == -1) {
            for (int i = 1; i < next_pid && i < MAX_PROCESSES; i++) {
                if (pid_to_process[i] == nullptr) {
                    child_pid = i;
                    next_pid = i + 1;
                    break;
                }
            }
        }
        
        if (child_pid == -1) {
            return -1;  
        }
        
        auto child_tcb = new UserProcessTCB(nullptr, child_pid);
        pid_to_process[child_pid] = child_tcb;
        
        child_tcb->parent_thread = parent_tcb;
        
        if (parent_tcb != nullptr) {
            auto parent_as_proc = static_cast<UserProcessTCB*>(parent_tcb);
            if (parent_as_proc->cwd != nullptr) {
                child_tcb->cwd = parent_as_proc->cwd;
            }
        }
        
        UserProcessTCB* parent_proc = nullptr;
        if (parent_tcb != nullptr) {
            for (int i = 0; i < MAX_PROCESSES; i++) {
                if (pid_to_process[i] != nullptr && 
                    (impl::threads::TCB*)pid_to_process[i] == parent_tcb) {
                    parent_proc = pid_to_process[i];
                    break;
                }
            }
            if (parent_proc == nullptr) {
                parent_proc = static_cast<UserProcessTCB*>(parent_tcb);
            }
        }
        
        if (parent_proc != nullptr) {
            child_tcb->next_sibling = parent_proc->first_child;
            parent_proc->first_child = child_tcb;
            child_tcb->has_parent_list = true;
        }
        
        child_tcb->pd = new_pd();
        
        uint32_t* parent_pd;
        if (parent_tcb != nullptr) {
            parent_pd = parent_tcb->pd;
            if (parent_pd == nullptr) {
                parent_pd = (uint32_t*)getCR3();
            }
        } else {
            parent_pd = (uint32_t*)getCR3();
        }
        
        for (uint32_t pdi = 0x80000000 >> 22; pdi < 0xF0000000 >> 22; pdi++) {
            uint32_t parent_pde = parent_pd[pdi];
            
            if (parent_pde & 1) {
                uint32_t parent_pt_pa = parent_pde & 0xFFFFF000;
                auto parent_pt = (uint32_t*)(parent_pt_pa);
                
                for (uint32_t pti = 0; pti < 1024; pti++) {
                    auto parent_pte = parent_pt[pti];
                    
                    if (parent_pte & 1) {
                        uint32_t parent_pa = parent_pte & 0xFFFFF000;
                        uint32_t child_pa = alloc_frame();
                        
                        memcpy((void*)child_pa, (void*)parent_pa, FRAME_SIZE);
                        
                        uint32_t va = (pdi << 22) | (pti << 12);
                        uint32_t bits = parent_pte & 0xFFF;
                        impl::VMM::map(child_tcb->pd, va, child_pa, bits);
                    }
                }
            }
        }
        
        using namespace impl::VMM;
        for (uint32_t pdi = 0xF0000000 >> 22; pdi < 1024; pdi++) {
            if (global_pd != nullptr && (global_pd[pdi] & 1)) {
                child_tcb->pd[pdi] = global_pd[pdi];
            }
        }
                
        if (parent_tcb != nullptr) {
            auto parent_vme = parent_tcb->vmes.first->next;  
            VME** child_vme_ptr = &child_tcb->vmes.first->next;
            
            while (parent_vme != nullptr) {
                auto new_vme = new VME(
                    parent_vme->range_start,
                    parent_vme->range_end,
                    parent_vme->file,
                    parent_vme->offset,
                    parent_vme->file_size
                );
                
                *child_vme_ptr = new_vme;
                child_vme_ptr = &new_vme->next;
                parent_vme = parent_vme->next;
            }
        }
        
        for (int i = 0; i < MAX_FDS; i++) {
            if (fd_table[i].in_use) {
                fd_table[i].refcount++;
            }
        }
        
        for (int i = 0; i < MAX_SEMS; i++) {
            if (sem_table[i].in_use) {
                sem_table[i].refcount++;
            }
        }
        
        extern void fork_child_return();  
        
        int stack_idx = (STACK_BYTES / sizeof(uintptr_t)) - 1;
        
        auto push = [&](uintptr_t val) {
            child_tcb->stack[stack_idx--] = val;
        };
        
        push(frame[4]);
        push(frame[3]);
        push(frame[2]);
        push(frame[1]);
        push(frame[0]);
        
        push((uint32_t)fork_child_return);
        push(0);
        push(0);
        push(0);
        push(0);
        push((uint32_t)child_tcb->pd);
        push(0);
        push(0x200);
        
        child_tcb->save_area.sp = (uintptr_t)&child_tcb->stack[stack_idx + 1];
        
        state.ready_queue.add(child_tcb);
                
        return child_pid;
    }
    
    case 3: /* sem */
    {
        uint32_t init_value = get_arg(frame, 0);
        
        int sem_index = -1;
        for (int i = 0; i < MAX_SEMS; i++) {
            if (!sem_table[i].in_use) {
                sem_index = i;
                break;
            }
        }
        
        if (sem_index == -1) {
            return -1;  
        }
        
        sem_table[sem_index].sem = StrongPtr<Semaphore>::make(init_value);
        sem_table[sem_index].refcount = 1;
        sem_table[sem_index].in_use = true;
        
        return MAX_FDS + sem_index;
    }
    
    case 4: /* up */
    {
        int sem_id = (int)get_arg(frame, 0);
        
        int sem_index = sem_id - MAX_FDS;
        
        if (sem_index < 0 || sem_index >= MAX_SEMS || !sem_table[sem_index].in_use) {
            return -1;  
        }
        
        sem_table[sem_index].sem->up();
        
        return 0;
    }
    
    case 5: /* down */
    {
        int sem_id = (int)get_arg(frame, 0);
        
        int sem_index = sem_id - MAX_FDS;
        
        if (sem_index < 0 || sem_index >= MAX_SEMS || !sem_table[sem_index].in_use) {
            return -1;  
        }
        
        sem_table[sem_index].sem->down();
        
        return 0;
    }
    
    case 6: // close
    {
        int id = (int)get_arg(frame, 0);
        
        if (id >= 3 && id < MAX_FDS && fd_table[id].in_use) {
            fd_table[id].refcount--;
            if (fd_table[id].refcount <= 0) {
                fd_table[id].in_use = false;
                fd_table[id].node = StrongPtr<Node>();
                fd_table[id].offset = 0;
                fd_table[id].refcount = 0;
            }
            return 0;
        }
        
        if (id >= MAX_FDS && id < MAX_FDS + MAX_SEMS) {
            int sem_index = id - MAX_FDS;
            if (sem_table[sem_index].in_use) {
                sem_table[sem_index].refcount--;
                if (sem_table[sem_index].refcount <= 0) {
                    sem_table[sem_index].sem = StrongPtr<Semaphore>();  
                    sem_table[sem_index].in_use = false;
                    sem_table[sem_index].refcount = 0;
                }
                return 0;
            }
        }
        
        if (id > 0 && id < MAX_PROCESSES && pid_to_process[id] != nullptr) {
            UserProcessTCB* child = pid_to_process[id];
            
            if (child->has_parent_list) {
                UserProcessTCB* parent = static_cast<UserProcessTCB*>(child->parent_thread);
                UserProcessTCB** child_ptr = &parent->first_child;
                while (*child_ptr != nullptr) {
                    if (*child_ptr == child) {
                        *child_ptr = child->next_sibling;
                        break;
                    }
                    child_ptr = &(*child_ptr)->next_sibling;
                }
                child->has_parent_list = false;
            }
            child->parent_thread = nullptr;
            return 0;
        }
        
        return -1;
    }
    
    case 7: /* shutdown */
        Debug::shutdown();
        return -1;

    case 8:  // wait
    {
        using namespace impl::threads;
        
        int child_pid = (int)get_arg(frame, 0);
        uint32_t* status_ptr = (uint32_t*)get_arg(frame, 1);
        
        auto current_tcb = state.current();
        if (current_tcb == nullptr) {
            return -1;
        }
        
        if (child_pid <= 0 || child_pid >= MAX_PROCESSES || pid_to_process[child_pid] == nullptr) {
            return -1;
        }
        
        UserProcessTCB* child = pid_to_process[child_pid];
        
        if (child->parent_thread != current_tcb) {
            return -1;  
        }
        
        UserProcessTCB** child_ptr = nullptr;
        if (child->has_parent_list) {
            UserProcessTCB* parent = static_cast<UserProcessTCB*>(current_tcb);
            child_ptr = &parent->first_child;
            while (*child_ptr != nullptr && *child_ptr != child) {
                child_ptr = &(*child_ptr)->next_sibling;
            }
            if (*child_ptr == nullptr) {
                return -1;
            }
        }
        
        if (child->state == PROC_ZOMBIE) {
            *status_ptr = child->exit_status;
            if (child_ptr != nullptr) {
                *child_ptr = child->next_sibling;
            }
            
            pid_to_process[child_pid] = nullptr;
            
            return 0;
        }
        
        UserProcessTCB* current = static_cast<UserProcessTCB*>(current_tcb);
        state.block("wait", [child, current] {
            exit_wait_lock.lock();
            
            if (child->state == PROC_ZOMBIE) {
                exit_wait_lock.unlock();
                state.ready_queue.add(current);
            } else {
                child->waiting_parent = current;
                exit_wait_lock.unlock();
            }
        });
        
        *status_ptr = child->exit_status;
        if (child_ptr != nullptr) {
            *child_ptr = child->next_sibling;
        }
        
        pid_to_process[child_pid] = nullptr;
        
        return 0;
    }
    
    case 9: // execl
    {
        using namespace impl::threads;
        using namespace impl::vme;
        using namespace PhysMem;
        
        uint32_t path_arg = get_arg(frame, 0);
        char* kernel_path = nullptr;
        
        if (path_arg != 0 && path_arg >= 0x80000000 && path_arg < 0xF0000000) {
            const char* user_path = (const char*)path_arg;
            int path_len = 0;
            while (path_len < 256 && user_path[path_len] != '\0') {
                path_len++;
            }
            
            if (path_len > 0 && path_len < 256) {
                kernel_path = new char[path_len + 1];
                for (int i = 0; i <= path_len; i++) {
                    kernel_path[i] = user_path[i];
                }
            }
        }
        
        const char* path = kernel_path ? kernel_path : (const char*)path_arg;
        
        if (path == nullptr || path[0] == '\0') {
            if (kernel_path) delete[] kernel_path;
            return -1;
        }
                
        if (global_fs == nullptr) {
            auto d = StrongPtr<Ide>::make(1,0);
            global_fs = StrongPtr<Ext2>::make(d);
        }
        
        StrongPtr<Node> start_dir = global_fs->root;
        auto current_tcb = state.current();
        if (current_tcb != nullptr) {
            auto current = static_cast<UserProcessTCB*>(current_tcb);
            if (current->cwd != nullptr && path[0] != '/') {
                start_dir = current->cwd;
            }
        }
        
        auto node = global_fs->find(start_dir, path);
        if (node == nullptr) {
            if (kernel_path) delete[] kernel_path;
            return -1;
        }
        
        int argc = 0;
        while (get_arg(frame, 1 + argc) != 0) {  
            argc++;
        }
        
        char** kernel_args = new char*[argc + 1];
        
        for (int i = 0; i < argc; i++) {
            const char* user_arg = (const char*)get_arg(frame, 1 + i);
            
            int len = 0;
            while (user_arg[len] != '\0') len++;
            
            kernel_args[i] = new char[len + 1];
            for (int j = 0; j <= len; j++) {
                kernel_args[i][j] = user_arg[j];
            }
        }
        kernel_args[argc] = nullptr;
        
        uint32_t* old_pd;
        if (current_tcb != nullptr) {
            old_pd = current_tcb->pd;
            
            auto vme = current_tcb->vmes.first->next;
            while (vme != nullptr) {
                auto next = vme->next;
                delete vme;
                vme = next;
            }
            current_tcb->vmes.first->next = nullptr;
        } else {
            old_pd = (uint32_t*)getCR3();
        }
        
        for (uint32_t pdi = 0x80000000 >> 22; pdi < 0xF0000000 >> 22; pdi++) {
            auto pde = old_pd[pdi];
            if (pde & 1) {
                auto pt = (uint32_t*)(pde & 0xFFFFF000);
                for (uint32_t pti = 0; pti < 1024; pti++) {
                    auto pte = pt[pti];
                    if (pte & 1) {
                        uint32_t pa = pte & 0xFFFFF000;
                        dealloc_frame(pa);
                        pt[pti] = 0;
                    }
                }
                dealloc_frame((uint32_t)pt);
                old_pd[pdi] = 0;
            }
        }
        
        setCR3((uint32_t)old_pd);
                
        uint32_t entry = ELF::load(node);
        
        if (entry == 0) {
            for (int i = 0; i < argc; i++) delete[] kernel_args[i];
            delete[] kernel_args;
            if (current_tcb != nullptr) {
                auto proc = static_cast<UserProcessTCB*>(current_tcb);
                
                exit_wait_lock.lock();
                proc->exit_status = 1;
                proc->state = PROC_ZOMBIE;
                
                UserProcessTCB* parent_to_wake = proc->waiting_parent;
                proc->waiting_parent = nullptr;
                exit_wait_lock.unlock();
                
                if (parent_to_wake != nullptr) {
                    impl::threads::state.ready_queue.add(parent_to_wake);
                }
                
                impl::threads::state.block("execl_failed", [proc] {
                });
            }
            return -1;
        }
        
        if (current_tcb != nullptr) {
            auto stack_vme = current_tcb->vmes.find(0xEFFFF000);
            if (stack_vme != nullptr) {
                current_tcb->vmes.remove(0xEFFFF000);
                delete stack_vme;
                
                auto large_stack = new VME(0xEFF00000, 0xF0000000, StrongPtr<Node>(), 0, 0);
                
                auto p = current_tcb->vmes.first;
                while (p->next != nullptr && p->next->range_start < 0xEFF00000) {
                    p = p->next;
                }
                large_stack->next = p->next;
                p->next = large_stack;
                
            }
        }
        
        uint32_t stack_va = 0xEFFFF000;
        uint32_t pdi = stack_va >> 22;
        uint32_t pti = (stack_va >> 12) & 0x3ff;
        
        uint32_t* current_pd = current_tcb ? current_tcb->pd : (uint32_t*)getCR3();
        auto pde = current_pd[pdi];
        if (!(pde & 1)) {
            for (int i = 0; i < argc; i++) delete[] kernel_args[i];
            delete[] kernel_args;
            return -1;
        }
        
        auto pt = (uint32_t*)(pde & 0xFFFFF000);
        auto pte = pt[pti];
        if (!(pte & 1)) {
            for (int i = 0; i < argc; i++) delete[] kernel_args[i];
            delete[] kernel_args;
            return -1;
        }
        
        uint32_t stack_pa = pte & 0xFFFFF000;
                
        bzero((void*)stack_pa, FRAME_SIZE);
        
        char* frame_base = (char*)stack_pa;
        
        uint32_t write_offset = 0x800;
        uint32_t* argv_addrs = new uint32_t[argc];
        
        for (int i = 0; i < argc; i++) {
            int len = 0;
            while (kernel_args[i][len] != '\0') len++;
            
            for (int j = 0; j <= len; j++) {
                frame_base[write_offset + j] = kernel_args[i][j];
            }
            
            argv_addrs[i] = stack_va + write_offset;
                        
            write_offset += ((len + 1) + 3) & ~3;
        }
        
        uint32_t argv_offset = write_offset;
        uint32_t* argv_array = (uint32_t*)(frame_base + argv_offset);
        for (int i = 0; i < argc; i++) {
            argv_array[i] = argv_addrs[i];
        }
        argv_array[argc] = 0;  
    
        uint32_t* stack_top = (uint32_t*)(frame_base + 0xFF0);
        stack_top[0] = argc;
        stack_top[1] = stack_va + argv_offset;  
        stack_top[2] = 0;                        
        
        for (int i = 0; i < argc; i++) delete[] kernel_args[i];
        delete[] kernel_args;
        delete[] argv_addrs;
        
        if (kernel_path) {
            delete[] kernel_path;
        }
        
        frame[0] = entry;         
        frame[3] = 0xEFFFFFF0;      
        
        return 0;  
    }
    
    case 10: // open
    {
        using namespace impl::threads;
        
        const char* path = (const char*)get_arg(frame, 0);
        int fd = -1;
        for (int i = 3; i < MAX_FDS; i++) {
            if (!fd_table[i].in_use) {
                fd = i;
                break;
            }
        }
        
        if (fd == -1) {
            return -1;
        }
        
        if (global_fs == nullptr) {
            auto d = StrongPtr<Ide>::make(1,0);
            global_fs = StrongPtr<Ext2>::make(d);
        }
        
        StrongPtr<Node> start_dir = global_fs->root;  
        auto current_tcb = state.current();
        if (current_tcb != nullptr) {
            auto current = static_cast<UserProcessTCB*>(current_tcb);
            if (current->cwd != nullptr && path[0] != '/') {
                start_dir = current->cwd;
            }
        }
        
        auto node = global_fs->find(start_dir, path);
        if (node == nullptr) {
            return -1;
        }
        
        fd_table[fd].node = node;
        fd_table[fd].offset = 0;
        fd_table[fd].refcount = 1;
        fd_table[fd].in_use = true;
        
        return fd;
    }
    
    case 11: // len
    {
        int fd = (int)get_arg(frame, 0);
        
        if (fd == 0 || fd == 1 || fd == 2) {
            return -1;
        }
        
        if (fd >= 0 && fd < MAX_FDS && fd_table[fd].in_use) {
            return (int)fd_table[fd].node->size_in_bytes();
        }
        
        return -1;
    }
    
    case 12: // read
    {
        int fd = (int)get_arg(frame, 0);
        void* buf = (void*)get_arg(frame, 1);
        uint32_t nbyte = get_arg(frame, 2);
        
        if (fd >= 0 && fd < MAX_FDS && fd_table[fd].in_use) {
            auto& fde = fd_table[fd];
            uint32_t file_size = fde.node->size_in_bytes();
            
            if (fde.offset >= file_size) {
                return 0; // EOF
            }
            
            uint32_t to_read = nbyte;
            if (fde.offset + to_read > file_size) {
                to_read = file_size - fde.offset;
            }
            
            int64_t result = fde.node->read_all(fde.offset, to_read, (char*)buf);
            if (result > 0) {
                fde.offset += result;
                return result;
            }
            
            return -1;
        }
        
        return -1;
    }
    
    case 13: // seek
    {
        int fd = (int)get_arg(frame, 0);
        uint32_t offset = get_arg(frame, 1);
        
        if (fd == 0 || fd == 1 || fd == 2) {
            return -1;
        }
        
        if (fd >= 0 && fd < MAX_FDS && fd_table[fd].in_use) {
            fd_table[fd].offset = offset;
            return offset;
        }
        
        return -1;
    }

    case 100: // chdir
    {
        using namespace impl::threads;
        
        const char* path = (const char*)get_arg(frame, 0);
        
        if (path == nullptr || path[0] == '\0') {
            return -1;
        }
        
        if (global_fs == nullptr) {
            auto d = StrongPtr<Ide>::make(1,0);
            global_fs = StrongPtr<Ext2>::make(d);
        }
        
        auto current_tcb = state.current();
        if (current_tcb == nullptr) {
            return -1;
        }
        auto current = static_cast<UserProcessTCB*>(current_tcb);
        
        StrongPtr<Node> start_dir = current->cwd;
        if (start_dir == nullptr) {
            start_dir = global_fs->root;
        }
        
        StrongPtr<Node> node = global_fs->find(start_dir, path);
        if (node == nullptr) {
            return -1;  
        }
        
        if (!node->is_dir()) {
            return -1;  
        }
        
        current->cwd = node;
        return 0;
    }
    
    case 101: // naive_mmap
    {
        uint32_t size = get_arg(frame, 0);
        int is_shared = (int)get_arg(frame, 1);
        int fd = (int)get_arg(frame, 2);
        uint32_t offset = get_arg(frame, 3);
        
        StrongPtr<Node> file_node;
        if (fd >= 0 && fd < MAX_FDS && fd_table[fd].in_use) {
            file_node = fd_table[fd].node;
        }
        
        void* result = VMM::naive_mmap(size, is_shared != 0, file_node, offset);
        return (int)result;
    }
    
    case 102: // naive_munmap
    {
        void* ptr = (void*)get_arg(frame, 0);
        VMM::naive_munmap(ptr);
        return 0;
    }
    
    case 103: // sleep
    {
        uint32_t seconds = get_arg(frame, 0);
        ::sleep(seconds);  
        return 0;
    }
    
    case 418: // iamateapot
    {
        Debug::printf("*** I'm a teapot\n");
        return 0;
    }

    default:
        Debug::printf("*** unknown system call %d\n",eax);
        return -1;
    }
    
}   

void SYS::init(void) {
    for (int i = 0; i < MAX_FDS; i++) {
        fd_table[i].in_use = false;
        fd_table[i].offset = 0;
        fd_table[i].refcount = 0;
    }
    
    for (int i = 0; i < MAX_SEMS; i++) {
        sem_table[i].in_use = false;
        sem_table[i].sem = StrongPtr<Semaphore>();
        sem_table[i].refcount = 0;
    }
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        pid_to_process[i] = nullptr;
    }
    
    IDT::trap(48,(uint32_t)sysHandler_,3);
}
