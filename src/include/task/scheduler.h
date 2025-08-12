#pragma once
#include <klib/types.h>
#include <task/man.h>

namespace Taskman {
  Task* spawn_process(void (*entry)(), const char* name, u32 priority = 1);
  void kill_process(u32 pid);
  void kill_current();
  
  Task* get_current();
  Task* get_by_pid(u32 pid);
  void yield();
  void sleep(u64 ticks);
  
  void start_scheduler();
  void stop_scheduler();
  
  void list_processes();
  u32 get_process_count();
  u32 get_active_count();
  
  void add_task(void (*entry)());
}
