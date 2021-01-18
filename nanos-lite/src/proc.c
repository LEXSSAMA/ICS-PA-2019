#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
extern void naive_uload(PCB *pcb, const char *filename) ;
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[1], "/bin/hello");

  context_kload(&pcb[0], (void *)hello_fun);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL,"/bin/init");
}

_Context* schedule(_Context *prev) {

// save the context pointer
  current->cp = prev;
// always select pcb[0] as the new process
// current =(current == &pcb[0] ? &pcb[1] : &pcb[0]);
  static int pcb_i=0;
  current = &pcb[pcb_i++];
  if(pcb_i==3)
  pcb_i=0;
// then return the new context
  return current->cp;
}
