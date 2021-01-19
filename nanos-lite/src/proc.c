#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
extern void naive_uload(PCB *pcb, const char *filename) ;
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);
PCB *current = NULL;
PCB *fg_pcb = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}
void switch_fg_pcb(int num){
  fg_pcb = &pcb[num];
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
  

  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL,"/bin/init");
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[3], "/bin/pal");
  fg_pcb = &pcb[1];
  context_kload(&pcb[0], (void *)hello_fun);
 
}

_Context* schedule(_Context *prev) {

// save the context pointer
  current->cp = prev;
// always select pcb[0] as the new process
  static int time_slice = 0;
  if(time_slice<=0){
    current =(current == &pcb[0] ? fg_pcb : &pcb[0]);
    time_slice = (current == &pcb[0])?10:20;
  }
  time_slice--;
// then return the new context
  return current->cp;
}

