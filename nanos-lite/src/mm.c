#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}
int mapped(PDE* dir_base,uint32_t vaddr){
    PDE dir_entry = dir_base[PDX(vaddr)];
    if((dir_entry&PTE_P)==0)
      return -1;
    PTE* ptab_base = (PTE*)PTE_ADDR(dir_entry);
    PTE ptab_entry = ptab_base[PTX(vaddr)];
    if((ptab_entry & PTE_P)==0)
      return -1;
    return 0;
}
/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  if(brk+increment<=current->max_brk){
    return -1;
  }
  // printf("brk==0x%x increment==0x%x, brk+increment==0x%x\n",brk,increment,brk+increment);
  if(current->max_brk==0){
    current->max_brk = brk;
  }
    uint32_t vaddr = current->max_brk;
    uint32_t size = brk+increment - vaddr;
    PDE offset = OFF(vaddr);
    uint32_t mapped_size = 0;
    if(offset>0&&mapped((PDE*)current->as.ptr,vaddr)!=0){
      void* phy_addr = new_page(1);
      _map(&current->as,(void*)vaddr,phy_addr,PTE_P);
      mapped_size += PGSIZE-offset;
      // printf("mm_brk : phy_addr == 0x%x\n",(uint32_t)phy_addr);
    }
      vaddr = ROUNDUP(vaddr,PGSIZE);
    while(mapped_size<size){
      void* phy_addr = new_page(1);
      _map(&current->as,(void*)vaddr,phy_addr,PTE_P);
      mapped_size+=PGSIZE;
      vaddr+=PGSIZE;
      // printf("mm_brk : phy_addr == 0x%x\n",(uint32_t)phy_addr);
    }
    current->max_brk = brk+increment;
    return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
