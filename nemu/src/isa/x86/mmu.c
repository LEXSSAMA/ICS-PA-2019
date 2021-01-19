#include "nemu.h"

// Page directory and page table constants
#define PGSIZE         4096    // Bytes mapped by a page
#define PGSHFT         12      // log2(PGSIZE)
#define PTXSHFT        12      // Offset of PTX in a linear address
#define PDXSHFT        22      // Offset of PDX in a linear address

// Page table/directory entry flags
#define PTE_P          0x001   // Present

typedef uint32_t PTE;
typedef uint32_t PDE;
#define PDX(va)          (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)          (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va)          ((uint32_t)(va) & 0xfff)
#define ROUNDUP(a, sz)   ((((uintptr_t)a)+(sz)-1) & ~((sz)-1))
#define ROUNDDOWN(a, sz) ((((uintptr_t)a)) & ~((sz)-1))
#define PTE_ADDR(pte)    ((uint32_t)(pte) & ~0xfff)
#define PGADDR(d, t, o)  ((uint32_t)((d) << PDXSHFT | (t) << PTXSHFT | (o)))


paddr_t page_translate(vaddr_t addr){
    if((cpu.cr[0]&0x80000000)==0){
        return addr;
    }
    PDE dir_base = (PDE) cpu.cr[3];
    PDE dir_entry = PTE_ADDR(dir_base) + PDX(addr)*4;
    PDE dir_entry_context = paddr_read(dir_entry,4);
    Assert(dir_entry_context&PTE_P,"The dir_entry (%d) don't exit ! , vaddr == 0x%x\n",PDX(addr),addr);
    PTE ptab_base =  ROUNDDOWN(dir_entry_context,PGSIZE);
    PTE ptab_entry = ptab_base + PTX(addr)*4;
    PTE ptab_entry_context = (PTE) paddr_read(ptab_entry,4);
    // if(addr>=0x4001a000&&addr<=0x4001afff){
    //     printf("vaddr==0x%x\ndir_base==0x%x\ndir_entry==0x%x\ndir_entry_context==0x%x\nptab_base==0x%x\nptab_entry==0x%x\nptab_entry_context==0x%x\n",
    //     addr,dir_base,dir_entry,dir_entry_context,ptab_base,ptab_entry,ptab_entry_context);
    //     printf("----------------------\n");
    // }
    Assert(ptab_entry_context&PTE_P,"vaddr==0x%x\ndir_base==0x%x\ndir_entry==0x%x\ndir_entry_context==0x%x\nptab_base==0x%x\nptab_entry==0x%x\nptab_entry_context==0x%x\n",
        addr,dir_base,dir_entry,dir_entry_context,ptab_base,ptab_entry,ptab_entry_context);
    // Assert(ptab_entry_context&PTE_P,"The page table entry (%d) don't exit !\n",PTX(addr));
    return PTE_ADDR(ptab_entry_context) | OFF(addr);
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
    //In this situation , We need to read two time Then splicing data together
    if (OFF(addr)+len>PGSIZE) {
        /* this is a special case, you can handle it later. */
        int byte_num = PGSIZE-OFF(addr);
        paddr_t paddr = page_translate(addr);
        uint32_t data = paddr_read(paddr,byte_num);
        paddr = page_translate(ROUNDUP(addr,PGSIZE));
        uint32_t data1 = paddr_read(paddr,(len-byte_num));
        return data|(data1<<(byte_num*8));
    }
    else {
        paddr_t paddr = page_translate(addr);
        return paddr_read(paddr, len);
    }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
    if (OFF(addr)+len>PGSIZE) {
        /* this is a special case, you can handle it later. */
        assert(0);
    }
    else {
        paddr_t paddr = page_translate(addr);
        paddr_write(paddr,data,len);
    }
}
