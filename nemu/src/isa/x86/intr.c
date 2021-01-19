#include "rtl/rtl.h"
#include "cpu/decode.h"
#define IRQ_TIMER 32     
extern DecodeInfo decinfo;

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
    Assert(NO*8<cpu.IDTR.idt_size,"intd %d Error: Overflow IDT table size !",NO);
    vaddr_t exce_intr_addr=0;
    uint32_t low=vaddr_read(cpu.IDTR.idt_addr+NO*8,4);
    uint32_t high=vaddr_read(cpu.IDTR.idt_addr+NO*8+4,4);
    Assert((high&0x00008000)!=0,"intd %d Error: The offset p equal to 0",NO);
    exce_intr_addr=(low&0xffff)|(high&0xffff0000);
    rtl_push(&cpu.flags.val);
    rtl_push(&cpu.cs);
    rtl_push(&ret_addr);
    cpu.flags.IF = 0;
    rtl_j(exce_intr_addr);
    return ;
}

bool isa_query_intr(void) {

  if (cpu.INTR&&cpu.flags.IF) {
    cpu.INTR = false;
    raise_intr(IRQ_TIMER,cpu.pc);
    return true;
  }

  return false;
}
