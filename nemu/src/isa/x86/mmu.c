#include "nemu.h"
paddr_t page_translate(vaddr_t addr){
    if((cpu.cr[0]&0x1)==0)
    Assert(0,"The Page-level mechanism isn't turn on\n");
    paddr_t dir_base = paddr_read(cpu.cr[3],4);
    paddr_t dir_entry = dir_base + (addr>>22)*4;
    uint32_t dir_entry_context = paddr_read(dir_entry,4);
    Assert((dir_entry_context&0x1)==1,"The dir_entry (%d) don't exit !\n",addr>>22);
    paddr_t ptab_base =  dir_entry_context & 0xffffff000;
    paddr_t ptab_entry = ptab_base + ((addr>>12) & 0x3ff)*4;
    uint32_t ptab_entry_context = paddr_read(ptab_entry,4);
    Assert((ptab_entry_context&0x1)==1,"The page table entry (%d) don't exit !\n",addr>>12 & 0x3ff);
    return (ptab_entry_context & 0xfffff000) + (addr & 0xfff);
}
uint32_t isa_vaddr_read(vaddr_t addr, int len) {
    if ((cpu.cr[0]&0x1)==0) {
        /* this is a special case, you can handle it later. */
        return paddr_read(addr,len);
    }
    else {
        paddr_t paddr = page_translate(addr);
        return paddr_read(paddr, len);
    }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
    if ((cpu.cr[0]&0x1)==0) {
        /* this is a special case, you can handle it later. */
        return paddr_write(addr,data,len);
    }
    else {
        paddr_t paddr = page_translate(addr);
        paddr_write(paddr,data,len);
    }
}
