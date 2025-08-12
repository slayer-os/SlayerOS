#include <task/man.h>
#include <task/scheduler.h>
#include <klib/types.h>
#include <klib/string.h>
#include <mem/heap.h>
#include <dbg/log.h>

constexpr usize STACK_SIZE = 32768;

static Taskman::ProcessList process_list = {nullptr, nullptr, nullptr, 0, 1};
static bool scheduler_running = false;

static void link_process(Taskman::Task* task) {
    if (!process_list.head) {
        process_list.head = task;
        process_list.tail = task;
        task->next = task;
        task->prev = task;
    } else {
        task->next = process_list.head;
        task->prev = process_list.tail;
        process_list.tail->next = task;
        process_list.head->prev = task;
        process_list.tail = task;
    }
    process_list.count++;
}

static void unlink_process(Taskman::Task* task) {
    if (process_list.count == 1) {
        process_list.head = nullptr;
        process_list.tail = nullptr;
        process_list.current = nullptr;
    } else {
        task->prev->next = task->next;
        task->next->prev = task->prev;
        
        if (process_list.head == task) {
            process_list.head = task->next;
        }
        if (process_list.tail == task) {
            process_list.tail = task->prev;
        }
        if (process_list.current == task) {
            process_list.current = task->next;
        }
    }
    process_list.count--;
}

static void cleanup_terminated() {
    if (!process_list.head) return;
    
    Taskman::Task* current = process_list.head;
    do {
        Taskman::Task* next = current->next;
        if (current->state == Taskman::State::TERMINATED) {
            unlink_process(current);
            kfree(current->stack);
            kfree(current);
        }
        current = next;
    } while (current != process_list.head && process_list.count > 0);
}

Taskman::Task* Taskman::spawn_process(void (*entry)(), const char* name, u32 priority) {
    Taskman::Task* task = (Taskman::Task*)kmalloc(sizeof(Taskman::Task));
    if (!task) return nullptr;
    
    task->stack = (u8*)kmalloc(STACK_SIZE);
    if (!task->stack) {
        kfree(task);
        return nullptr;
    }
    
    u64 stack_top = (u64)task->stack + STACK_SIZE;
    
    memset(&task->ctx, 0, sizeof(Taskman::Context));
    task->ctx.rip = (u64)entry;
    task->ctx.cs = 0x08;
    task->ctx.ss = 0x10;
    task->ctx.rflags = 0x202;
    task->ctx.rsp = stack_top;
    
    task->pid = process_list.next_pid++;
    task->state = Taskman::State::READY;
    task->name = name;
    task->priority = priority;
    task->start_time = 0; // TODO: Add timer support
    task->cpu_time = 0;
    
    link_process(task);
    
    return task;
}

void Taskman::kill_process(u32 pid) {
    Taskman::Task* task = get_by_pid(pid);
    if (task) {
        task->state = Taskman::State::TERMINATED;
    }
}

void Taskman::kill_current() {
    if (process_list.current) {
        process_list.current->state = Taskman::State::TERMINATED;
        yield();
    }
}

Taskman::Task* Taskman::get_current() {
    return process_list.current;
}

Taskman::Task* Taskman::get_by_pid(u32 pid) {
    if (!process_list.head) return nullptr;
    
    Taskman::Task* current = process_list.head;
    do {
        if (current->pid == pid) {
            return current;
        }
        current = current->next;
    } while (current != process_list.head);
    
    return nullptr;
}

void Taskman::yield() {
    cleanup_terminated();
    
    if (!process_list.current || process_list.count == 0) {
        return;
    }
    
    Taskman::Task* next = process_list.current->next;
    u32 checked = 0;
    
    while (checked < process_list.count && 
           (next->state != Taskman::State::ACTIVE && next->state != Taskman::State::READY)) {
        next = next->next;
        checked++;
    }
    
    if (checked >= process_list.count) {
        return;
    }
    
    if (next == process_list.current) {
        return;
    }
    
    process_list.current = next;
    next->state = Taskman::State::ACTIVE;
    
    void (*task_func)() = (void(*)())next->ctx.rip;
    task_func();
}

void Taskman::sleep(u64 ticks) {
    if (process_list.current) {
        process_list.current->state = Taskman::State::WAITING;
        // TODO: Add timer-based wakeup
        yield();
    }
}

void Taskman::start_scheduler() {
    if (process_list.count == 0) {
        return;
    }
    
    scheduler_running = true;
    
    Taskman::Task* current = process_list.head;
    while (current && current->state != Taskman::State::READY && current->state != Taskman::State::ACTIVE) {
        current = current->next;
        if (current == process_list.head) break;
    }
    
    if (!current) return;
    
    process_list.current = current;
    current->state = Taskman::State::ACTIVE;
    
    void (*task_func)() = (void(*)())current->ctx.rip;
    task_func();
}

void Taskman::stop_scheduler() {
    scheduler_running = false;
}

void Taskman::list_processes() {
    if (!process_list.head) {
        Log::info("No processes running");
        return;
    }
    
    Log::info("Process List (%d total):", process_list.count);
    Taskman::Task* current = process_list.head;
    do {
        const char* state_str = "UNKNOWN";
        switch (current->state) {
            case Taskman::State::ACTIVE: state_str = "ACTIVE"; break;
            case Taskman::State::READY: state_str = "READY"; break;
            case Taskman::State::WAITING: state_str = "WAITING"; break;
            case Taskman::State::TERMINATED: state_str = "TERMINATED"; break;
        }
        
        Log::info("  PID %d: %s [%s] Priority: %d", 
                  current->pid, current->name, state_str, current->priority);
        current = current->next;
    } while (current != process_list.head);
}

u32 Taskman::get_process_count() {
    return process_list.count;
}

u32 Taskman::get_active_count() {
    if (!process_list.head) return 0;
    
    u32 active = 0;
    Taskman::Task* current = process_list.head;
    do {
        if (current->state == Taskman::State::ACTIVE || current->state == Taskman::State::READY) {
            active++;
        }
        current = current->next;
    } while (current != process_list.head);
    
    return active;
}

void Taskman::add_task(void (*entry)()) {
    spawn_process(entry, "legacy_task", 1);
}
