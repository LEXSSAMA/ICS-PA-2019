#include "nemu.h"
#include "monitor/diff-test.h"
bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for(int i=0;i<8;++i){
    if(ref_r->gpr[i]._32!=reg_l(i))
      return false;
  }
  if(ref_r->pc!=cpu.pc)
      return false;
  return true;
} 

void isa_difftest_attach(void) {
  ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
  ref_difftest_memcpy_from_dut(PC_START, guest_to_host(IMAGE_START), PMEM_SIZE-IMAGE_START);
  /*In here we can not write `ref_difftest_memcpy_from_dut(0x7e00,&cpu.IDTR,6)` directly!
  The reason I guess may be Align,(I am not sure !)*/
  ref_difftest_memcpy_from_dut(0x7e00,&cpu.IDTR.idt_size,2);
  ref_difftest_memcpy_from_dut(0x7e02,&cpu.IDTR.idt_addr,4);
  uint8_t code[] = {0x0f,0x01,0x18};
  ref_difftest_memcpy_from_dut(0x7e40,code,sizeof(code));
  CPU_state ref ;
  ref_difftest_getregs(&ref);
  ref.pc=0x7e40;
  ref.eax=0x7e00;
  ref_difftest_setregs(&ref);
  ref_difftest_exec(1);

  ref_difftest_setregs(&cpu);
}
