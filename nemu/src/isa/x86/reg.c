#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

//编写打印寄存器的基础设施
void isa_reg_display(char* args) {
  char *arg = strtok(NULL," ");
  if(arg[0]=='r')
  {
  printf("EAX           0x%x    %d\n",cpu.gpr[R_EAX]._32,cpu.gpr[R_EAX]._32);
  printf(" AX           0x%x    %d\n",cpu.gpr[R_AX]._16,cpu.gpr[R_AX]._16);
  printf(" AH           0x%x    %d\n",cpu.gpr[R_AH&0x3]._8[1],cpu.gpr[R_AH&0x3]._8[1]);
  printf(" AL           0x%x    %d\n",cpu.gpr[R_AL&0x3]._8[0],cpu.gpr[R_AL&0x3]._8[0]);

  printf("EDX           0x%x    %d\n",cpu.gpr[R_EDX]._32,cpu.gpr[R_EDX]._32);
  printf(" DX           0x%x    %d\n",cpu.gpr[R_DX]._16,cpu.gpr[R_DX]._16);
  printf(" DH           0x%x    %d\n",cpu.gpr[R_DH&0x3]._8[1],cpu.gpr[R_DH&0x3]._8[1]);
  printf(" DL           0x%x    %d\n",cpu.gpr[R_DL&0x3]._8[0],cpu.gpr[R_DL&0x3]._8[0]);

  printf("ECX           0x%x    %d\n",cpu.gpr[R_ECX]._32,cpu.gpr[R_ECX]._32);
  printf(" CX           0x%x    %d\n",cpu.gpr[R_CX]._16,cpu.gpr[R_CX]._16);
  printf(" CH           0x%x    %d\n",cpu.gpr[R_CH&0x3]._8[1],cpu.gpr[R_CH&0x3]._8[1]);
  printf(" CL           0x%x    %d\n",cpu.gpr[R_CL&0x3]._8[0],cpu.gpr[R_CL&0x3]._8[0]);

  printf("EBX           0x%x    %d\n",cpu.gpr[R_EBX]._32,cpu.gpr[R_EBX]._32);
  printf(" BX           0x%x    %d\n",cpu.gpr[R_BX]._16,cpu.gpr[R_BX]._16);
  printf(" BH           0x%x    %d\n",cpu.gpr[R_BH&0x3]._8[1],cpu.gpr[R_BH&0x3]._8[1]);
  printf(" BL           0x%x    %d\n",cpu.gpr[R_BL&0x3]._8[0],cpu.gpr[R_BL&0x3]._8[0]);


  printf("ESP           0x%x    %d\n",cpu.gpr[R_ESP]._32,cpu.gpr[R_ESP]._32);
  printf(" SP           0x%x    %d\n",cpu.gpr[R_SP]._32,cpu.gpr[R_SP]._32);

  printf("ESI           0x%x    %d\n",cpu.gpr[R_ESI]._32,cpu.gpr[R_ESI]._32);
  printf(" SI           0x%x    %d\n",cpu.gpr[R_SI]._32,cpu.gpr[R_SI]._32);

  printf("EDI           0x%x    %d\n",cpu.gpr[R_EDI]._32,cpu.gpr[R_EDI]._32);
  printf(" DI           0x%x    %d\n",cpu.gpr[R_DI]._32,cpu.gpr[R_DI]._32);
  }

 }

uint32_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}

