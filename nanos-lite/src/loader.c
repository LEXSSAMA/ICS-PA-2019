#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern uint8_t ramdisk_start;
static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr* elfheader = (Elf_Ehdr*)&ramdisk_start;
  Elf_Phdr* pro_header = (Elf_Phdr*)(elfheader->e_phoff+&ramdisk_start);
  uint32_t ph_num = elfheader->e_phnum;
  for(int i=0;i<ph_num;++i){
   if(pro_header->p_type!=PT_LOAD) {
      pro_header++;
      continue;
   }
    ramdisk_read((void*)pro_header->p_vaddr,pro_header->p_offset,pro_header->p_filesz);
    if(pro_header->p_filesz<pro_header->p_memsz)
    memset((void*)(pro_header->p_vaddr+pro_header->p_filesz),0,pro_header->p_memsz-pro_header->p_filesz);
    pro_header++;
  }
  return elfheader->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
