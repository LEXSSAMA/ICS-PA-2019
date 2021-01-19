#include "common.h"

extern _Context* do_syscall(_Context *c);
_Context* schedule(_Context *prev) ;

static _Context* do_event(_Event e, _Context* c) {
  _Context* result =NULL;
  switch (e.event) {
    case _EVENT_YIELD: result = schedule(c); break;
    case _EVENT_SYSCALL: result = do_syscall(c); break;
    case _EVENT_IRQ_TIMER: _yield(); Log("Recevice time interrupt!\n"); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  return result;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
