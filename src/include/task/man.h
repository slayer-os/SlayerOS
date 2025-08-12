#pragma once
#include <klib/types.h>

namespace Taskman {
  struct Context {
      u64 r15, r14, r13, r12, r11, r10, r9, r8;
      u64 rsi, rdi, rbp, rdx, rcx, rbx, rax;
      u64 rip, cs, rflags, rsp, ss;
  };

  enum State {
    ACTIVE,
    WAITING,
    TERMINATED,
    READY
  };

  struct Task {
      Context ctx;
      u8* stack;
      u32 pid;
      State state;
      Task* next;
      Task* prev;
      const char* name;
      u32 priority;
      u64 start_time;
      u64 cpu_time;
  };

  struct ProcessList {
      Task* head;
      Task* tail;
      Task* current;
      u32 count;
      u32 next_pid;
  };
}

extern "C" void task_switch(Taskman::Context *current, Taskman::Context *next);
