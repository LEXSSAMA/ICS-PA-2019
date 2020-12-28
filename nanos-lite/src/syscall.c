#include "common.h"
#include "syscall.h"

int sys_yield(){
  _yield();
  return 0;
}
int sys_halt(int code){
  _halt(code);
}
_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_yield: c->GPRx=sys_yield(); break;
    case SYS_exit: sys_halt(c->GPRx);  break;
    default: panic("Unhandled syscall ID = %d\n", a[0]);
  }
  return c;
}
