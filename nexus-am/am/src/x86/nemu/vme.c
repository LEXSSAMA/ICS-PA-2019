#include <am.h>
#include <x86.h>
#include <nemu.h>
#include <klib.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
//The Physical memory can be divided to (PMEM_SIZE + MMIO_SIZE) / PGSIZE pages
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static _Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE},
  {.start = (void*)MMIO_BASE,  .end = (void*)(MMIO_BASE + MMIO_SIZE)}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    //here is calculate how many dir the physical memory used .
    //(PGSIZE * NR_PTE) Byte represent how many Byte a dir have 
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE (ptab value will change when finishing a cycle )
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }
  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;

  return 0;
}

int _protect(_AddressSpace *as) {
  // Apply a page memory for PDE
  PDE *updir = (PDE*)(pgalloc_usr(1));
  as->ptr = updir;
  // map kernel space
  /*code of AM , navy and nanos is kernel code start at 0x100000 as Kernel code , user process is start at 0x40000000 , 
  if this code does not exist (mean: Don't map kernel code), The error will occur when eip pointer to kernel code*/
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  return 0;
}

void _unprotect(_AddressSpace *as) {
}

static _AddressSpace *cur_as = NULL;
void __am_get_cur_as(_Context *c) {
  c->as = cur_as;
}

void __am_switch(_Context *c) {
  if (vme_enable) {
    set_cr3(c->as->ptr);
    cur_as = c->as;
  }
}

int _map(_AddressSpace *as, void *va, void *pa, int prot) {
  PDE* pdir_base = (PDE*)as->ptr;
  PTE* ptab_base = NULL;
  uint32_t vaddr = (uint32_t)va;

  if((pdir_base[PDX(vaddr)] & 0x1) == 0){
    PDE pdir_entry = (PDE) pgalloc_usr(1) ;
    pdir_base[PDX(vaddr)] = ROUNDDOWN(pdir_entry,PGSIZE) | prot; 
    }
  ptab_base = (PTE*) (pdir_base[PDX(vaddr)] & (~0x3ff));
  ptab_base[PTX(vaddr)] = ROUNDDOWN(pa,PGSIZE) | prot;
  // if((uint32_t)va==0x4001a000){
  // printf("dir_base ==0x%x \nptab_base == 0x%x\nptab_entry==0x%x\nptab_entry_context == 0x%x\n"
  // ,pdir_base,ptab_base,&ptab_base[PTX(vaddr)],ptab_base[PTX(vaddr)]);
  // }
  return 0;
}

_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args) {
  _Context* uct = (_Context*)(ustack.end-sizeof(_Context)-3*sizeof(uint32_t));
  uct->as = as;
  uct->eip  = (uintptr_t) entry;
  uct->cs = 8;
  return uct;
}
