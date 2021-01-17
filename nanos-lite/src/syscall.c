#include "common.h"
#include "syscall.h"
#include "proc.h"

extern char _end,_heap_end;
static uint32_t program_break = (uint32_t)&_end;
static uint32_t heap_end = (uint32_t)&_heap_end;
extern size_t fs_write(int fd,const void* buf,size_t len);
extern size_t fs_read(int fd,const void* buf,size_t len);
extern int fs_open(const char* pathname,int flags,int mode);
extern int fs_close(int fd);
extern int fs_lseek(int fd,int offset,int whence);
extern void naive_uload(PCB *pcb, const char *filename) ;

uint32_t sys_brk(intptr_t increment){
    uint32_t prev_program_break = program_break;
    if(program_break+increment>=heap_end)
         return -1;
    program_break+=increment;
    return prev_program_break;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: _yield(); break;
    case SYS_exit:   _halt(0); c->GPRx=0; break;
    case SYS_execve: naive_uload(NULL,(const char*)a[1]); c->GPRx=0; break;
    case SYS_write: c->GPRx=fs_write(a[1],(void*)a[2],a[3]); break;
    case SYS_brk:   c->GPRx=sys_brk(a[1]); break;
    case SYS_read:  c->GPRx=fs_read(a[1],(void*)a[2],a[3]); break;
    case SYS_open:  c->GPRx=fs_open((const char*)a[1],a[2],a[3]); break;
    case SYS_close: c->GPRx=fs_close(a[1]); break;
    case SYS_lseek: c->GPRx=fs_lseek(a[1],a[2],a[3]);break;
    default: panic("Unhandled syscall ID = %d\n", a[0]);
  }
  return c;
}
