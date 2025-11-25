#include "debug.h"
#include "smp.h"
#include "debug.h"
#include "config.h"
#include "pit.h"
#include "threads.h"
#include "atomic.h"
#include "blocking_queue.h"
#include "vmm.h"
#include "tss.h"

namespace impl::threads {

    TCB::~TCB() {
        using namespace PhysMem;

        if (pd == nullptr) return;

        ASSERT(uint32_t(pd) != getCR3());

        // clear private range
        for (uint32_t pdi=0x8000'0000>>22; pdi < 0xF000'0000>>22; pdi += 1) {
            auto pde = pd[pdi];
            if (pde & 1) {
                auto pt = (uint32_t*)(framedown(pde));
                for (uint32_t pti=0; pti < 1024; pti += 1) {
                    auto pte = pt[pti];
                    if (pte & 1) {
                        dealloc_frame(framedown(pte));
                    }
                }
                dealloc_frame(uint32_t(pt));
            }
        }
        dealloc_frame(uint32_t(pd));
    }

    // State constructor
    State::State() {

        reaper_queue = new BlockingQueue<TCB>();

        // The reaper thread, needs to run in a thread in order to
        // be able to access the heap and contend for the queue
        thread([this] {
            while(true) {
                auto p = reaper_queue->remove();
                ASSERT(p != nullptr);
                delete p;
            }
        });
    }

    State state{};

    void reap() {
        auto p = state.reaper_queue->remove_all();
        while (p != nullptr) {
            auto next = p->next;
            delete p;
            p = next;
        }
    }

    void SleepQueue::add(TCB* tcb, uint32_t after_seconds) {
        ASSERT(tcb != nullptr);                    // must have a TCB
        ASSERT(state.in_helper_thread());    // can only run with preemption disabled

        TCB** pprev = &first;
        TCB* p = first;

        tcb->at_jiffies = Pit::jiffies + Pit::secondsToJiffies(after_seconds);

        while ((p != nullptr) && (p->at_jiffies <= tcb->at_jiffies)) {
            pprev = &p->next;
            p = p->next;
        }

        tcb->next = *pprev;
        *pprev = tcb;
    }

    TCB* SleepQueue::remove() {
        ASSERT(state.in_helper_thread()); // can only run in the helper thread with preemption disabled
        auto p = first;

        if ((p == nullptr) || (p->at_jiffies > Pit::jiffies)) {
            return nullptr;
        } else {
            first = p->next;
            p->next = nullptr;
            return p;
        }
    }

    [[noreturn]]
    void thread_entry() {
        ASSERT(!Interrupts::isDisabled());
        auto me = state.current();
        ASSERT(me != nullptr);
        me->doit();
        stop();
    }
    

    [[noreturn]]
    void helper() {
        // Enable paging
        vmm_on(uint32_t(impl::VMM::global_pd));

        // Sanity checks
        auto id = SMP::me();
        ASSERT(state.in_helper_thread());

        auto wakeup = [id] {
            auto p = state.sleep_queues[id].remove();
            while (p != nullptr) {
                state.ready_queue.add(p);
                p = state.sleep_queues[id].remove();
            }
        };

        while (true) {
            ASSERT(state.in_helper_thread());      // nullptr -> helper thread
            ASSERT(!Interrupts::isDisabled());         // interrupts should be enabled
            ASSERT(SMP::me() == id);                   // has affinity to a core


            wakeup();

            auto request = state.help_requests[id];
            if (request != nullptr) {
                state.help_requests[id] = nullptr;
                request->doit();
            }

            TCB* next = state.ready_queue.remove();
            while (next == nullptr) {
                iAmStuckInALoop(false);
                wakeup();
                next = state.ready_queue.remove();
            }

            // setting active_thread enables preemption, need to disable interrupts briefly
            cli();     
            state.active_thread[id] = next;                         // O(1)
	    tss[id].esp0 = next->interruptEsp();                    // O(1)
            context_switch(&state.helpers[id], &next->save_area);   // O(1)
            sti();   
        }
    }

    uint32_t* new_pd() {
        auto pd = (uint32_t*)PhysMem::alloc_frame();
        
        // Zero out the entire page directory first
        bzero((void*)pd, 4096);

        for (uint32_t i=0; i<1024; i++) {
            auto pte = impl::VMM::global_pd[i];
            if (pte & 1) {
                pd[i] = pte;
            }
        }

        return pd;
    }
}

using namespace impl::threads;

void yield() {
    // Performance optimization. In the worse case, the data race will
    // cause us to miss a ready thread once but will find it when we
    // check again.
    reap();

    if (state.ready_queue.isEmpty()) return;


    auto tcb = state.current();
    ASSERT(tcb != nullptr);
    state.block("yield", [tcb] {
        // run in the helper thread
        state.ready_queue.add(tcb);
    });
}

void sleep(uint32_t sec) {
    reap();
    auto tcb = state.current();
    ASSERT(tcb != nullptr);
    state.block("sleep",[tcb, sec] {
        // run in helper thread with preemption disabled
        state.sleep_queues[SMP::me()].add(tcb, sec);
    });
}

[[noreturn]]
void stop() {
    auto tcb = state.current();
    if (tcb == nullptr) {
        // only the initial cores call stop with current thread set to nullptr, they
        // morph into the helper threads
        helper();
    } else {
        reap();
        // a created thread is attempting to stop
        state.block("stop", [tcb] {
            // run in helper thread, can't delete here because delete uses the heap
            // and that uses a blocking lock but the helper thread is not allowed to block
            state.reaper_queue->add(tcb);
        });
        PANIC("the impossible has happened");
    }
}


