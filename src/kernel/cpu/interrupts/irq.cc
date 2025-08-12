#include <cpu/irq.h>
#include <cpu/idt.h>
#include <cpu/isr.h>
#include <cpu/tss.h>
#include <cpu/lapic.h>
#include <cpu/pit.h>
#include <sys/ioapic.h>
#include <dbg/log.h>

void *irq_handlers[256] = {0};
extern void *isr_handlers[256];

static u64 timer_interrupt_count = 0;

void IRQ::register_entry(u8 irq, void (*handler)(struct irq_frame*)) {
  irq_handlers[irq] = (void*)handler;
  IDT::register_entry_with_ist(irq, (u64)isr_handlers[irq], 0x08, IDT::INT_GATE, TSS::ISTIndex::IST_IRQ);
}

extern "C" void irq_handler(struct irq_frame *frame) {
  if (irq_handlers[frame->error]) {
    void (*handler)(struct irq_frame*) = (void (*)(struct irq_frame*))irq_handlers[frame->error];
    handler(frame);
  }
  
  LAPIC::eoi();
}

void Timer_Handle(struct irq_frame *) {
  timer_interrupt_count++;
  PIT::tick();
}

void IOAPIC_Handle(struct irq_frame *frame) {
  Log::printf("IOAPIC IRQ %d\n", frame->error);
}

void IRQ::init() {
  IRQ::register_entry(0x20, Timer_Handle);
  irq_handlers[0] = (void*)Timer_Handle;
  
  Sys::IOAPIC::set(0x20, 0, 0, LAPIC::id());
  
  Log::success("IRQ handlers initialized");
}
