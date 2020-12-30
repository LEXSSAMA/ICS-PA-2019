#include "common.h"
#include "syscall.h"
extern size_t fs_write(int fd,const void* buf,size_t len);
int sys_write(int fd,void* buf,size_t count){
  return fs_write(fd,buf,count);
}
int sys_yield(){
  _yield();
  return 0;
}
int sys_halt(int code){
  _halt(code);
}
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
    case SYS_yield: c->GPRx=sys_yield(); break;
    case SYS_exit:  sys_halt(c->GPRx);  break;
    case SYS_write: c->GPRx=sys_write(a[1],(void*)a[2],a[3]); break;
    case SYS_brk:   c->GPRx=sys_brk(a[1]); break;
    default: panic("Unhandled syscall ID = %d\n", a[0]);
  }
  return c;
}
