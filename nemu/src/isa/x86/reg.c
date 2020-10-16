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
  printf("\033[0;36mEAX           0x%x    %d\033[0;m\n",cpu.gpr[R_EAX]._32,cpu.gpr[R_EAX]._32);
  printf("\033[0;36m AX           0x%x    %d\033[0;m\n",cpu.gpr[R_AX]._16,cpu.gpr[R_AX]._16);
  printf("\033[0;36m AH           0x%x    %d\033[0;m\n",cpu.gpr[R_AH&0x3]._8[1],cpu.gpr[R_AH&0x3]._8[1]);
  printf("\033[0;36m AL           0x%x    %d\033[0;m\n",cpu.gpr[R_AL&0x3]._8[0],cpu.gpr[R_AL&0x3]._8[0]);

  printf("\033[0;36mEDX           0x%x    %d\033[0;m\n",cpu.gpr[R_EDX]._32,cpu.gpr[R_EDX]._32);
  printf("\033[0;36m DX           0x%x    %d\033[0;m\n",cpu.gpr[R_DX]._16,cpu.gpr[R_DX]._16);
  printf("\033[0;36m DH           0x%x    %d\033[0;m\n",cpu.gpr[R_DH&0x3]._8[1],cpu.gpr[R_DH&0x3]._8[1]);
  printf("\033[0;36m DL           0x%x    %d\033[0;m\n",cpu.gpr[R_DL&0x3]._8[0],cpu.gpr[R_DL&0x3]._8[0]);

  printf("\033[0;36mECX           0x%x    %d\033[0;m\n",cpu.gpr[R_ECX]._32,cpu.gpr[R_ECX]._32);
  printf("\033[0;36m CX           0x%x    %d\033[0;m\n",cpu.gpr[R_CX]._16,cpu.gpr[R_CX]._16);
  printf("\033[0;36m CH           0x%x    %d\033[0;m\n",cpu.gpr[R_CH&0x3]._8[1],cpu.gpr[R_CH&0x3]._8[1]);
  printf("\033[0;36m CL           0x%x    %d\033[0;m\n",cpu.gpr[R_CL&0x3]._8[0],cpu.gpr[R_CL&0x3]._8[0]);

  printf("\033[0;36mEBX           0x%x    %d\033[0;m\n",cpu.gpr[R_EBX]._32,cpu.gpr[R_EBX]._32);
  printf("\033[0;36m BX           0x%x    %d\033[0;m\n",cpu.gpr[R_BX]._16,cpu.gpr[R_BX]._16);
  printf("\033[0;36m BH           0x%x    %d\033[0;m\n",cpu.gpr[R_BH&0x3]._8[1],cpu.gpr[R_BH&0x3]._8[1]);
  printf("\033[0;36m BL           0x%x    %d\033[0;m\n",cpu.gpr[R_BL&0x3]._8[0],cpu.gpr[R_BL&0x3]._8[0]);


  printf("\033[0;36mESP           0x%x    %d\033[0;m\n",cpu.gpr[R_ESP]._32,cpu.gpr[R_ESP]._32);
  printf("\033[0;36m SP           0x%x    %d\033[0;m\n",cpu.gpr[R_SP]._16,cpu.gpr[R_SP]._16);

  printf("\033[0;36mESI           0x%x    %d\033[0;m\n",cpu.gpr[R_ESI]._32,cpu.gpr[R_ESI]._32);
  printf("\033[0;36m SI           0x%x    %d\033[0;m\n",cpu.gpr[R_SI]._16,cpu.gpr[R_SI]._16);

  printf("\033[0;36mEDI           0x%x    %d\033[0;m\n",cpu.gpr[R_EDI]._32,cpu.gpr[R_EDI]._32);
  printf("\033[0;36m DI           0x%x    %d\033[0;m\n",cpu.gpr[R_DI]._16,cpu.gpr[R_DI]._16);
  }

 }

uint32_t isa_reg_str2val(const char *s, bool *success) {
  if(strcasecmp(s,"pc")==0)
      return cpu.pc;
  for(int i=R_EAX;i<=R_EDI;++i)
    if(strcasecmp(s,regsl[i])==0)
      return reg_l(i);
  for(int i=R_AX;i<=R_DI;++i)
    if(strcasecmp(s,regsw[i])==0)
      return reg_w(i);
  for(int i=R_AL;i<=R_BH;++i)
      if(strcasecmp(s,regsb[i])==0)
        return reg_b(i);
  *success=false;
  return 0;
}

