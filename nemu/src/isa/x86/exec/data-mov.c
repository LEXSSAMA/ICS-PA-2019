#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&s0);
  operand_write(id_dest,&s0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  s0 = reg_l(R_ESP);
  rtl_push(&reg_l(R_EAX));
  rtl_push(&reg_l(R_ECX));
  rtl_push(&reg_l(R_EDX));
  rtl_push(&reg_l(R_EBX));
  rtl_push(&s0);
  rtl_push(&reg_l(R_EBP));
  rtl_push(&reg_l(R_ESI));
  rtl_push(&reg_l(R_EDI));
  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&reg_l(R_EDI));
  rtl_pop(&reg_l(R_ESI));
  rtl_pop(&reg_l(R_EBP));
  rtl_pop(&s0);     //(skip ESP)
  rtl_pop(&reg_l(R_EBX));
  rtl_pop(&reg_l(R_EDX));
  rtl_pop(&reg_l(R_ECX));
  rtl_pop(&reg_l(R_EAX));
  print_asm("popa");
}

make_EHelper(leave) {
  rtl_lr(&s0,R_EBP,4);
  rtl_sr(R_ESP,&s0,4);
  rtl_pop(&s0);
  rtl_sr(R_EBP,&s0,4);
  print_asm("leave");
}
//Convert double word to Quadword  or Convert word to double word;
make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_lr(&s1,R_AX,2);
    rtl_sext(&s0,&s1,2);
    rtl_sari(&s1,&s0,16);
    rtl_sr(R_DX,&s1,2);
  }
  else {
    rtl_lr(&s0,R_EAX,4);
    rtl_sari(&s1,&s0,31);
    rtl_sr(R_EDX,&s1,4);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

// Convert word to double word or Convert byte to word;
make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
      rtl_lr(&s1,R_AL,1);
      rtl_sext(&s0,&s1,1);
      rtl_sr(R_AX,&s0,2);
  }
  else {
    rtl_lr(&s1,R_AX,2);
    rtl_sext(&s0,&s1,2);
    rtl_sr(R_EAX,&s0,4);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}
make_EHelper(movsb){
  rtl_lr(&s0,R_ESI,4);
  rtl_lm(&s1,&s0,4);
  rtl_sm(&reg_l(R_EDI),&s1,1);
  reg_l(R_ESI)+=1;
  reg_l(R_EDI)+=1;
}
make_EHelper(movsl){
 s1 = 4;
 if(decinfo.isa.is_operand_size_16) {
   s1 = 2;
   rtl_lr(&s0,R_ESI,4);
   rtl_lm(&s0,&s0,4);
   rtl_sm(&reg_l(R_EDI),&s0,2);
 }
 else{
   rtl_lr(&s0,R_ESI,4);
   rtl_lm(&s0,&s0,4);
   rtl_sm(&reg_l(R_EDI),&s0,4);
 }
   reg_l(R_ESI)+=s1;
   reg_l(R_EDI)+=s1;
}
make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
