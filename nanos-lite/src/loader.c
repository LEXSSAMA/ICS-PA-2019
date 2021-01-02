#include "proc.h"
#include <elf.h>
#include "fs.h"

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern uint8_t ramdisk_start;
extern int fs_open(const char* pathname,int flags,int mode);
extern int fs_read(int fd, void *buf,int len);
extern int fs_lseek(int fd,int offset,int whence);
extern int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elfheader;
  Elf_Phdr pro_header;

  int fd = fs_open(filename,0,0);
  fs_lseek(fd,0,SEEK_SET);
  fs_read(fd,&elfheader,sizeof(Elf_Ehdr));
  uint32_t ph_num = elfheader.e_phnum;

  for(int i=0;i<ph_num;++i){

    fs_lseek(fd,elfheader.e_phoff+i*elfheader.e_phentsize,SEEK_SET);
    fs_read(fd,&pro_header,sizeof(Elf_Phdr));
    if(pro_header.p_type!=PT_LOAD)
      continue;
    fs_lseek(fd,pro_header.p_offset,SEEK_SET);
    fs_read(fd,(void*)pro_header.p_vaddr,pro_header.p_filesz);
    if(pro_header.p_filesz<pro_header.p_memsz)
    memset((void*)(pro_header.p_vaddr+pro_header.p_filesz),0,pro_header.p_memsz-pro_header.p_filesz);

  }

  fs_close(fd);
  return elfheader.e_entry;
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
