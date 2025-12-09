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
#include "pit.h"

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
    uint32_t program_break;  // For brk() syscall
    char cwd_path[256];      // For getcwd() syscall
    FilesystemOverlay* fs_overlay;  // Per-process filesystem modifications
    
    UserProcessTCB(uint32_t* pd, int pid) 
        : TCB(pd), pid(pid), parent_thread(nullptr), first_child(nullptr), 
          next_sibling(nullptr), state(PROC_RUNNING), exit_status(0), 
          waiting_parent(nullptr), cwd(nullptr), has_parent_list(false),
          program_break(0), fs_overlay(nullptr) {
        cwd_path[0] = '/';
        cwd_path[1] = '\0';
        fs_overlay = new FilesystemOverlay();
    }
    
    ~UserProcessTCB() {
        if (fs_overlay) {
            delete fs_overlay;
        }
    }
    
    void doit() override {
        Debug::panic("UserProcessTCB::doit() called");
    }
    
    uint32_t interruptEsp() override {
        return uint32_t(&stack[(STACK_BYTES / sizeof(uintptr_t)) - 1]);
    }
};

static inline uint32_t get_arg(uint32_t *frame, int n) {
    uint32_t *user_stack = (uint32_t*)(frame[11]); 
    return user_stack[n];
}

void sysInit(StrongPtr<Ext2> fs) {
    global_fs = fs;
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
    
    case 4: // Linux write
    case 1:  // write 
    {
        int fd = (int)get_arg(frame, 0);
        const char* buf = (const char*)get_arg(frame, 1);
        uint32_t nbyte = get_arg(frame, 2);
        
        if (fd == 1 || fd == 2) {
            for (uint32_t i = 0; i < nbyte; i++) {
                Debug::printf("%c", buf[i]);
            }
            return nbyte;
        }

        if (fd >= 0 && fd < MAX_FDS && fd_table[fd].in_use) {
            auto& fde = fd_table[fd];
            int64_t written = fde.node->write(fde.offset, nbyte, buf);
            if (written < 0) return -1;
            
            fde.offset += written;
            return written;
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
        
        push(frame[12]); // SS
        push(frame[11]); // ESP
        push(frame[10]); // EFLAGS
        push(frame[9]); // CS
        push(frame[8]); // EIP
        
        push((uint32_t)fork_child_return);
        push(frame[4]); // EBX
        push(frame[1]); // ESI
        push(frame[0]); // EDI
        push(frame[2]); // EBP
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
    
    case 1004: /* up */
    {
        int sem_id = (int)get_arg(frame, 0);
        
        int sem_index = sem_id - MAX_FDS;
        
        if (sem_index < 0 || sem_index >= MAX_SEMS || !sem_table[sem_index].in_use) {
            return -1;  
        }
        
        sem_table[sem_index].sem->up();
        
        return 0;
    }
    
    case 1005: /* down */
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
    
    case 1000: // shutdown
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
    
    // ========== NEW P8 SYSCALLS ==========
    
    case 20: // getpid - Get process ID
    {
        using namespace impl::threads;
        auto current_tcb = state.current();
        if (current_tcb == nullptr) {
            // No thread - return PID 1 for init
            Debug::printf("getpid: no TCB, returning 1\n");
            return 1;
        }
        
        // Check if this is a UserProcessTCB by looking in pid_to_process table
        UserProcessTCB* user_proc = nullptr;
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (pid_to_process[i] != nullptr && 
                (impl::threads::TCB*)pid_to_process[i] == current_tcb) {
                user_proc = pid_to_process[i];
                break;
            }
        }
        
        if (user_proc != nullptr) {
            Debug::printf("getpid: found user_proc, returning %d\n", user_proc->pid);
            return user_proc->pid;
        }
        
        // Not a user process (kernel thread) - return PID 1 for init
        Debug::printf("getpid: not a user process, returning 1\n");
        return 1;
    }
    
    case 64: // getppid - Get parent process ID
    {
        using namespace impl::threads;
        auto current_tcb = state.current();
        if (current_tcb == nullptr) {
            return -1;
        }
        auto current = static_cast<UserProcessTCB*>(current_tcb);
        if (current->parent_thread == nullptr) {
            return 0;  // No parent
        }
        // Find parent's PID
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (pid_to_process[i] != nullptr && 
                (impl::threads::TCB*)pid_to_process[i] == current->parent_thread) {
                return pid_to_process[i]->pid;
            }
        }
        return 0;
    }
    
    case 106: // stat - Get file status by path
    {
        using namespace impl::threads;
        
        const char* path = (const char*)get_arg(frame, 0);
        void* statbuf = (void*)get_arg(frame, 1);
        
        if (path == nullptr || statbuf == nullptr) {
            return -1;
        }
        
        if (global_fs == nullptr) {
            auto d = StrongPtr<Ide>::make(1,0);
            global_fs = StrongPtr<Ext2>::make(d);
        }
        
        // Get current process
        auto current_tcb = state.current();
        UserProcessTCB* current = nullptr;
        if (current_tcb != nullptr) {
            for (int i = 0; i < MAX_PROCESSES; i++) {
                if (pid_to_process[i] != nullptr && 
                    (impl::threads::TCB*)pid_to_process[i] == current_tcb) {
                    current = pid_to_process[i];
                    break;
                }
            }
        }
        
        // Check if deleted in overlay
        if (current && current->fs_overlay && current->fs_overlay->is_deleted(path)) {
            return -1;  // File deleted
        }
        
        // Check if exists in overlay
        if (current && current->fs_overlay) {
            FsModification* overlay_entry = current->fs_overlay->find_entry(path);
            if (overlay_entry) {
                // Return fake stat info for overlay entry
                struct stat_struct {
                    uint32_t st_dev;
                    uint32_t st_ino;
                    uint16_t st_mode;
                    uint16_t st_nlink;
                    uint16_t st_uid;
                    uint16_t st_gid;
                    uint32_t st_rdev;
                    uint32_t st_size;
                    uint32_t st_blksize;
                    uint32_t st_blocks;
                    uint32_t st_atime;
                    uint32_t st_mtime;
                    uint32_t st_ctime;
                };
                
                stat_struct* st = (stat_struct*)statbuf;
                st->st_dev = 0;
                st->st_ino = overlay_entry->fake_inode;
                st->st_mode = overlay_entry->mode;
                st->st_nlink = 1;
                st->st_uid = 0;
                st->st_gid = 0;
                st->st_rdev = 0;
                st->st_size = overlay_entry->size;
                st->st_blksize = 4096;
                st->st_blocks = (overlay_entry->size + 511) / 512;
                st->st_atime = 0;
                st->st_mtime = 0;
                st->st_ctime = 0;
                return 0;  // Success
            }
        }
        
        StrongPtr<Node> start_dir = global_fs->root;
        if (current_tcb != nullptr) {
            auto current_proc = static_cast<UserProcessTCB*>(current_tcb);
            if (current_proc->cwd != nullptr && path[0] != '/') {
                start_dir = current_proc->cwd;
            }
        }
        
        auto node = global_fs->find(start_dir, path);
        if (node == nullptr) {
            return -1;
        }
        
        // Fill in stat structure
        struct stat_struct {
            uint32_t st_dev;
            uint32_t st_ino;
            uint16_t st_mode;
            uint16_t st_nlink;
            uint16_t st_uid;
            uint16_t st_gid;
            uint32_t st_rdev;
            uint32_t st_size;
            uint32_t st_blksize;
            uint32_t st_blocks;
            uint32_t st_atime;
            uint32_t st_mtime;
            uint32_t st_ctime;
        };
        
        stat_struct* st = (stat_struct*)statbuf;
        st->st_dev = 0;
        st->st_ino = node->number;
        st->st_mode = node->get_type();
        st->st_nlink = node->n_links();
        st->st_uid = 0;
        st->st_gid = 0;
        st->st_rdev = 0;
        st->st_size = node->size_in_bytes();
        st->st_blksize = 4096;
        st->st_blocks = (node->size_in_bytes() + 511) / 512;
        st->st_atime = 0;
        st->st_mtime = 0;
        st->st_ctime = 0;
        
        return 0;
    }
    
    case 108: // fstat - Get file status by file descriptor
    {
        int fd = (int)get_arg(frame, 0);
        void* statbuf = (void*)get_arg(frame, 1);
        
        if (statbuf == nullptr) {
            return -1;
        }
        
        if (fd < 0 || fd >= MAX_FDS || !fd_table[fd].in_use) {
            return -1;
        }
        
        auto node = fd_table[fd].node;
        if (node == nullptr) {
            return -1;
        }
        
        struct stat_struct {
            uint32_t st_dev;
            uint32_t st_ino;
            uint16_t st_mode;
            uint16_t st_nlink;
            uint16_t st_uid;
            uint16_t st_gid;
            uint32_t st_rdev;
            uint32_t st_size;
            uint32_t st_blksize;
            uint32_t st_blocks;
            uint32_t st_atime;
            uint32_t st_mtime;
            uint32_t st_ctime;
        };
        
        stat_struct* st = (stat_struct*)statbuf;
        st->st_dev = 0;
        st->st_ino = node->number;
        st->st_mode = node->get_type();
        st->st_nlink = node->n_links();
        st->st_uid = 0;
        st->st_gid = 0;
        st->st_rdev = 0;
        st->st_size = node->size_in_bytes();
        st->st_blksize = 4096;
        st->st_blocks = (node->size_in_bytes() + 511) / 512;
        st->st_atime = 0;
        st->st_mtime = 0;
        st->st_ctime = 0;
        
        return 0;
    }
    
    case 107: // lstat - Get file status (no symlink follow)
    {
        // For now, same as stat since we don't have symlinks
        using namespace impl::threads;
        
        const char* path = (const char*)get_arg(frame, 0);
        void* statbuf = (void*)get_arg(frame, 1);
        
        if (path == nullptr || statbuf == nullptr) {
            return -1;
        }
        
        if (global_fs == nullptr) {
            auto d = StrongPtr<Ide>::make(1,0);
            global_fs = StrongPtr<Ext2>::make(d);
        }
        
        // Get current process
        auto current_tcb = state.current();
        UserProcessTCB* current = nullptr;
        if (current_tcb != nullptr) {
            for (int i = 0; i < MAX_PROCESSES; i++) {
                if (pid_to_process[i] != nullptr && 
                    (impl::threads::TCB*)pid_to_process[i] == current_tcb) {
                    current = pid_to_process[i];
                    break;
                }
            }
        }
        
        // Check if deleted in overlay
        if (current && current->fs_overlay && current->fs_overlay->is_deleted(path)) {
            return -1;  // File deleted
        }
        
        // Check if exists in overlay
        if (current && current->fs_overlay) {
            FsModification* overlay_entry = current->fs_overlay->find_entry(path);
            if (overlay_entry) {
                // Return fake stat info for overlay entry
                struct stat_struct {
                    uint32_t st_dev;
                    uint32_t st_ino;
                    uint16_t st_mode;
                    uint16_t st_nlink;
                    uint16_t st_uid;
                    uint16_t st_gid;
                    uint32_t st_rdev;
                    uint32_t st_size;
                    uint32_t st_blksize;
                    uint32_t st_blocks;
                    uint32_t st_atime;
                    uint32_t st_mtime;
                    uint32_t st_ctime;
                };
                
                stat_struct* st = (stat_struct*)statbuf;
                st->st_dev = 0;
                st->st_ino = overlay_entry->fake_inode;
                st->st_mode = overlay_entry->mode;
                st->st_nlink = 1;
                st->st_uid = 0;
                st->st_gid = 0;
                st->st_rdev = 0;
                st->st_size = overlay_entry->size;
                st->st_blksize = 4096;
                st->st_blocks = (overlay_entry->size + 511) / 512;
                st->st_atime = 0;
                st->st_mtime = 0;
                st->st_ctime = 0;
                return 0;  // Success
            }
        }
        
        StrongPtr<Node> start_dir = global_fs->root;
        if (current_tcb != nullptr) {
            auto current_proc = static_cast<UserProcessTCB*>(current_tcb);
            if (current_proc->cwd != nullptr && path[0] != '/') {
                start_dir = current_proc->cwd;
            }
        }
        
        auto node = global_fs->find(start_dir, path);
        if (node == nullptr) {
            return -1;
        }
        
        struct stat_struct {
            uint32_t st_dev;
            uint32_t st_ino;
            uint16_t st_mode;
            uint16_t st_nlink;
            uint16_t st_uid;
            uint16_t st_gid;
            uint32_t st_rdev;
            uint32_t st_size;
            uint32_t st_blksize;
            uint32_t st_blocks;
            uint32_t st_atime;
            uint32_t st_mtime;
            uint32_t st_ctime;
        };
        
        stat_struct* st = (stat_struct*)statbuf;
        st->st_dev = 0;
        st->st_ino = node->number;
        st->st_mode = node->get_type();
        st->st_nlink = node->n_links();
        st->st_uid = 0;
        st->st_gid = 0;
        st->st_rdev = 0;
        st->st_size = node->size_in_bytes();
        st->st_blksize = 4096;
        st->st_blocks = (node->size_in_bytes() + 511) / 512;
        st->st_atime = 0;
        st->st_mtime = 0;
        st->st_ctime = 0;
        
        return 0;
    }
    
    case 141: // getdents - Get directory entries
    {
        int fd = (int)get_arg(frame, 0);
        void* dirp = (void*)get_arg(frame, 1);
        uint32_t count = get_arg(frame, 2);
        
        if (dirp == nullptr || count == 0) {
            return -1;
        }
        
        if (fd < 0 || fd >= MAX_FDS || !fd_table[fd].in_use) {
            return -1;
        }
        
        auto node = fd_table[fd].node;
        if (node == nullptr || !node->is_dir()) {
            return -1;
        }
        
        struct linux_dirent {
            uint32_t d_ino;
            uint32_t d_off;
            uint16_t d_reclen;
            char d_name[256];
        };
        
        char* buf = (char*)dirp;
        uint32_t bytes_written = 0;
        uint32_t entry_index = fd_table[fd].offset;
        
        // Read directory entries using read_all
        uint32_t dir_size = node->size_in_bytes();
        char* dir_data = new char[dir_size];
        int64_t read_result = node->read_all(0, dir_size, dir_data);
        
        if (read_result <= 0) {
            delete[] dir_data;
            return -1;
        }
        
        // Parse ext2 directory entries
        uint32_t offset = 0;
        uint32_t current_entry = 0;
        
        while (offset < dir_size && bytes_written < count) {
            struct ext2_dir_entry {
                uint32_t inode;
                uint16_t rec_len;
                uint8_t name_len;
                uint8_t file_type;
                char name[255];
            } __attribute__((packed));
            
            ext2_dir_entry* ext2_entry = (ext2_dir_entry*)(dir_data + offset);
            
            if (ext2_entry->inode == 0 || ext2_entry->rec_len == 0) {
                break;
            }
            
            // Skip entries until we reach the current offset
            if (current_entry >= entry_index) {
                uint32_t name_len = ext2_entry->name_len;
                uint32_t reclen = sizeof(uint32_t) * 2 + sizeof(uint16_t) + name_len + 1;
                reclen = (reclen + 3) & ~3;  // Align to 4 bytes
                
                if (bytes_written + reclen > count) {
                    break;
                }
                
                linux_dirent* d = (linux_dirent*)(buf + bytes_written);
                d->d_ino = ext2_entry->inode;
                d->d_off = current_entry + 1;
                d->d_reclen = reclen;
                
                for (uint32_t i = 0; i < name_len; i++) {
                    d->d_name[i] = ext2_entry->name[i];
                }
                d->d_name[name_len] = '\0';
                
                bytes_written += reclen;
            }
            
            offset += ext2_entry->rec_len;
            current_entry++;
        }
        
        delete[] dir_data;
        
        fd_table[fd].offset = entry_index;
        
        return bytes_written;
    }
    
    case 183: // getcwd - Get current working directory
    {
        using namespace impl::threads;
        
        char* buf = (char*)get_arg(frame, 0);
        uint32_t size = get_arg(frame, 1);
        
        if (buf == nullptr || size == 0) {
            return -1;
        }
        
        auto current_tcb = state.current();
        if (current_tcb == nullptr) {
            return -1;
        }
        auto current = static_cast<UserProcessTCB*>(current_tcb);
        
        // Copy cwd_path to user buffer
        uint32_t i = 0;
        while (current->cwd_path[i] != '\0' && i < size - 1) {
            buf[i] = current->cwd_path[i];
            i++;
        }
        buf[i] = '\0';
        
        return (int)buf;  // Return pointer to buf
    }
    
    case 45: // brk - Set program break
    {
        using namespace impl::threads;
        
        uint32_t new_brk = get_arg(frame, 0);
        
        auto current_tcb = state.current();
        if (current_tcb == nullptr) {
            return -1;
        }
        auto current = static_cast<UserProcessTCB*>(current_tcb);
        
        // If new_brk is 0, return current break
        if (new_brk == 0) {
            return current->program_break;
        }
        
        // For now, just update the break point
        // In a full implementation, you'd allocate/deallocate pages
        current->program_break = new_brk;
        
        return 0;  // Success
    }
    
    case 78: // gettimeofday - Get time of day
    {
        void* tv = (void*)get_arg(frame, 0);
        
        if (tv == nullptr) {
            return -1;
        }
        
        struct timeval {
            uint32_t tv_sec;
            uint32_t tv_usec;
        };
        
        timeval* time_ptr = (timeval*)tv;
        
        // Use Pit for time
        uint32_t current_seconds = Pit::seconds();
        time_ptr->tv_sec = current_seconds;
        // Calculate microseconds from the jiffy remainder
        // Assuming 1000 jiffies per second (1 ms resolution), convert to microseconds
        time_ptr->tv_usec = (Pit::jiffies % Pit::secondsToJiffies(1)) * 1000;
        
        return 0;
    }
    
    case 162: // nanosleep - Sleep with nanosecond precision
    {
        void* req = (void*)get_arg(frame, 0);
        
        if (req == nullptr) {
            return -1;
        }
        
        struct timespec {
            uint32_t tv_sec;
            uint32_t tv_nsec;
        };
        
        timespec* ts = (timespec*)req;
        
        // For now, just sleep for the seconds part
        if (ts->tv_sec > 0) {
            ::sleep(ts->tv_sec);
        }
        
        return 0;
    }
    
    case 125: // mprotect - Change memory protection
    {
        // For now, this is a stub - just return success
        // In a full implementation, you'd modify page table entries
        // to change read/write/execute permissions
        
        // TODO: Actually modify page table entries based on prot flags
        // PROT_READ = 1, PROT_WRITE = 2, PROT_EXEC = 4
        
        return 0;  // Success (stub)
    }
    
    case 39: // mkdir - Create directory
    {
        using namespace impl::threads;
        const char* pathname = (const char*)get_arg(frame, 0);
        uint32_t mode = get_arg(frame, 1);
        
        if (pathname == nullptr) {
            return -1;
        }
        
        auto current_tcb = state.current();
        if (current_tcb == nullptr) {
            return -1;
        }
        
        // Find the UserProcessTCB
        UserProcessTCB* current = nullptr;
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (pid_to_process[i] != nullptr && 
                (impl::threads::TCB*)pid_to_process[i] == current_tcb) {
                current = pid_to_process[i];
                break;
            }
        }
        
        if (current == nullptr || current->fs_overlay == nullptr) {
            return -1;
        }
        
        // Check if path already exists (in overlay or on disk)
        if (current->fs_overlay->find_entry(pathname) != nullptr) {
            return -1;  // Already exists
        }
        
        // Try to find it in the real filesystem
        StrongPtr<Node> start_dir = global_fs->root;
        if (current->cwd != nullptr && pathname[0] != '/') {
            start_dir = current->cwd;
        }
        StrongPtr<Node> existing = global_fs->find(start_dir, pathname);
        if (existing != nullptr) {
            return -1;  // Already exists on disk
        }
        
        // Create directory in overlay
        if (current->fs_overlay->create_directory(pathname, mode)) {
            return 0;  // Success
        }
        
        return -1;  // Failed
    }
    
    case 40: // rmdir - Remove directory
    {
        using namespace impl::threads;
        const char* pathname = (const char*)get_arg(frame, 0);
        
        if (pathname == nullptr) {
            return -1;
        }
        
        auto current_tcb = state.current();
        if (current_tcb == nullptr) {
            return -1;
        }
        
        // Find the UserProcessTCB
        UserProcessTCB* current = nullptr;
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (pid_to_process[i] != nullptr && 
                (impl::threads::TCB*)pid_to_process[i] == current_tcb) {
                current = pid_to_process[i];
                break;
            }
        }
        
        if (current == nullptr || current->fs_overlay == nullptr) {
            return -1;
        }
        
        // Check if already deleted
        if (current->fs_overlay->is_deleted(pathname)) {
            return -1;  // Already deleted
        }
        
        // Check if exists (in overlay or on disk)
        FsModification* overlay_entry = current->fs_overlay->find_entry(pathname);
        
        StrongPtr<Node> start_dir = global_fs->root;
        if (current->cwd != nullptr && pathname[0] != '/') {
            start_dir = current->cwd;
        }
        StrongPtr<Node> disk_node = global_fs->find(start_dir, pathname);
        
        if (overlay_entry == nullptr && disk_node == nullptr) {
            return -1;  // Doesn't exist
        }
        
        // Check if it's a directory
        if (overlay_entry) {
            if (overlay_entry->type != FsModification::CREATE_DIR) {
                return -1;  // Not a directory
            }
        } else if (disk_node != nullptr && !disk_node->is_dir()) {
            return -1;  // Not a directory
        }
        
        // Delete directory in overlay
        if (current->fs_overlay->delete_entry(pathname)) {
            return 0;  // Success
        }
        
        return -1;  // Failed
    }
    
    case 87: // unlink - Delete file
    {
        using namespace impl::threads;
        const char* pathname = (const char*)get_arg(frame, 0);
        
        if (pathname == nullptr) {
            return -1;
        }
        
        auto current_tcb = state.current();
        if (current_tcb == nullptr) {
            return -1;
        }
        
        // Find the UserProcessTCB
        UserProcessTCB* current = nullptr;
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (pid_to_process[i] != nullptr && 
                (impl::threads::TCB*)pid_to_process[i] == current_tcb) {
                current = pid_to_process[i];
                break;
            }
        }
        
        if (current == nullptr || current->fs_overlay == nullptr) {
            return -1;
        }
        
        // Check if already deleted
        if (current->fs_overlay->is_deleted(pathname)) {
            return -1;  // Already deleted
        }
        
        // Check if exists (in overlay or on disk)
        FsModification* overlay_entry = current->fs_overlay->find_entry(pathname);
        
        StrongPtr<Node> start_dir = global_fs->root;
        if (current->cwd != nullptr && pathname[0] != '/') {
            start_dir = current->cwd;
        }
        StrongPtr<Node> disk_node = global_fs->find(start_dir, pathname);
        
        if (overlay_entry == nullptr && disk_node == nullptr) {
            return -1;  // Doesn't exist
        }
        
        // Check if it's NOT a directory
        if (overlay_entry) {
            if (overlay_entry->type == FsModification::CREATE_DIR) {
                return -1;  // Is a directory, use rmdir
            }
        } else if (disk_node != nullptr && disk_node->is_dir()) {
            return -1;  // Is a directory, use rmdir
        }
        
        // Delete file in overlay
        if (current->fs_overlay->delete_entry(pathname)) {
            return 0;  // Success
        }
        
        return -1;  // Failed
    }
    
    case 38: // rename - Rename/move file or directory
    {
        using namespace impl::threads;
        const char* oldpath = (const char*)get_arg(frame, 0);
        const char* newpath = (const char*)get_arg(frame, 1);
        
        if (oldpath == nullptr || newpath == nullptr) {
            return -1;
        }
        
        auto current_tcb = state.current();
        if (current_tcb == nullptr) {
            return -1;
        }
        
        // Find the UserProcessTCB
        UserProcessTCB* current = nullptr;
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (pid_to_process[i] != nullptr && 
                (impl::threads::TCB*)pid_to_process[i] == current_tcb) {
                current = pid_to_process[i];
                break;
            }
        }
        
        if (current == nullptr || current->fs_overlay == nullptr) {
            return -1;
        }
        
        // Check if old path exists
        FsModification* overlay_entry = current->fs_overlay->find_entry(oldpath);
        
        StrongPtr<Node> start_dir = global_fs->root;
        if (current->cwd != nullptr && oldpath[0] != '/') {
            start_dir = current->cwd;
        }
        StrongPtr<Node> disk_node = global_fs->find(start_dir, oldpath);
        
        if (overlay_entry == nullptr && disk_node == nullptr) {
            return -1;  // Old path doesn't exist
        }
        
        // Check if old path is deleted
        if (current->fs_overlay->is_deleted(oldpath)) {
            return -1;  // Old path deleted
        }
        
        // Rename in overlay
        if (current->fs_overlay->rename_entry(oldpath, newpath)) {
            return 0;  // Success
        }
        
        return -1;  // Failed
    }

    case 90: // mmap (old_mmap)
    {
        struct mmap_arg_struct {
            uint32_t addr;
            uint32_t len;
            uint32_t prot;
            uint32_t flags;
            uint32_t fd;
            uint32_t offset;
        } *args = (struct mmap_arg_struct*)get_arg(frame, 0);
        
        if (args == nullptr) return -1;
        
        uint32_t len = args->len;
        uint32_t flags = args->flags;
        int fd = (int)args->fd;
        uint32_t offset = args->offset;
        
        StrongPtr<Node> node;
        if (!(flags & 0x20)) { // Not MAP_ANONYMOUS
            if (fd >= 0 && fd < MAX_FDS && fd_table[fd].in_use) {
                node = fd_table[fd].node;
            } else {
                return -1; // EBADF
            }
        }
        
        bool shared = (flags & 0x01); // MAP_SHARED
        
        void* ret = VMM::naive_mmap(len, shared, node, offset);
        if (ret == nullptr) return -1; // ENOMEM
        return (int)ret;
    }

    case 192: // mmap2
    {
        // uint32_t addr = get_arg(frame, 0); // Hint, ignored
        uint32_t len = get_arg(frame, 1);
        // uint32_t prot = get_arg(frame, 2);
        uint32_t flags = get_arg(frame, 3);
        int fd = (int)get_arg(frame, 4);
        uint32_t pgoff = get_arg(frame, 5);
        
        StrongPtr<Node> node;
        if (!(flags & 0x20)) { // Not MAP_ANONYMOUS
            if (fd >= 0 && fd < MAX_FDS && fd_table[fd].in_use) {
                node = fd_table[fd].node;
            } else {
                return -1; // EBADF
            }
        }
        
        bool shared = (flags & 0x01); // MAP_SHARED
        
        void* ret = VMM::naive_mmap(len, shared, node, pgoff * 4096);
        if (ret == nullptr) return -1; // ENOMEM
        return (int)ret;
    }

    case 19: // lseek
    {
        int fd = (int)get_arg(frame, 0);
        int offset = (int)get_arg(frame, 1);
        int whence = (int)get_arg(frame, 2);
        
        if (fd >= 0 && fd < MAX_FDS && fd_table[fd].in_use) {
            auto& fde = fd_table[fd];
            uint32_t file_size = fde.node->size_in_bytes();
            
            if (whence == 0) { // SEEK_SET
                fde.offset = offset;
            } else if (whence == 1) { // SEEK_CUR
                fde.offset += offset;
            } else if (whence == 2) { // SEEK_END
                fde.offset = file_size + offset;
            } else {
                return -1;
            }
            
            return fde.offset;
        }
        return -1;
    }

    case 118: // fsync
    {
        int fd = (int)get_arg(frame, 0);
        
        if (fd >= 0 && fd < MAX_FDS && fd_table[fd].in_use) {
            fd_table[fd].node->sync();
            return 0;
        }
        return -1;
    }

    case 7: // waitpid
    {
        using namespace impl::threads;
        int pid = (int)get_arg(frame, 0);
        uint32_t* status = (uint32_t*)get_arg(frame, 1);
        
        if (pid < 0 || pid >= MAX_PROCESSES || pid_to_process[pid] == nullptr) return -1;
        
        auto child = pid_to_process[pid];
        auto current = static_cast<UserProcessTCB*>(state.current());
        
        if (child->parent_thread != current) return -1;
        
        exit_wait_lock.lock();
        if (child->state == PROC_ZOMBIE) {
            if (status) *status = child->exit_status;
            exit_wait_lock.unlock();
            pid_to_process[pid] = nullptr;
            return pid;
        }
        
        child->waiting_parent = current;
        exit_wait_lock.unlock();
        
        state.block("waitpid", [] { });
        
        if (status) *status = child->exit_status;
        pid_to_process[pid] = nullptr;
        return pid;
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
