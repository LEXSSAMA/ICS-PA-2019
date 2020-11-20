#include "cpu/exec.h"
#include "cc.h"

//Test operation discards the result;
make_EHelper(test) {
  rtl_and(&s0,&id_dest->val,&id_src->val);
  if(id_dest->width!=4)
  {
    rtl_andi(&s0,&s0,(0xffffffffu>>((4-id_dest->width)*8)));
  }
  rtl_update_ZFSF(&s0,id_dest->width);
  s1 = 0;
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  if(id_dest->width!=4)
  {
    rtl_andi(&s0,&s0,(0xffffffffu>>((4-id_dest->width)*8)));
  }
  rtl_update_ZFSF(&s0,id_dest->width);
  // "and" operation doesn't affect ZF and CF flags
  s1 = 0;
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  if(id_dest->width!=4)
  {
    rtl_andi(&s0,&s0,(0xffffffffu>>((4-id_dest->width)*8)));
  }
  rtl_update_ZFSF(&s0,id_dest->width);
  s1 = 0;
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  //更新ZFSF
  if(id_dest->width!=4)
  {
    rtl_andi(&s0,&s0,(0xffffffffu>>((4-id_dest->width)*8)));
  }
  rtl_update_ZFSF(&s0,id_dest->width);
  //更新CF和OF,因为是或操作所以不会有进位和溢出的情况
  s1 = 0;
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);
  print_asm_template2(or);
}

make_EHelper(sar) {
   rtl_sext(&s0,&id_dest->val,id_dest->width);
   rtl_sar(&s0,&s0,&id_src->val);
   operand_write(id_dest,&s0);
  if(id_dest->width!=4)
  {
    rtl_andi(&s0,&s0,(0xffffffffu>>((4-id_dest->width)*8)));
  }
   rtl_update_ZFSF(&s0,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
    rtl_shl(&s0,&id_dest->val,&id_src->val);
    operand_write(id_dest,&s0);
  // unnecessary to update CF and OF in NEMU
    if(id_dest->width!=4)
    {
      rtl_andi(&s0,&s0,(0xffffffffu>>((4-id_dest->width)*8)));
    }
    rtl_update_ZFSF(&s0,id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  // unnecessary to update CF and OF in NEMU
  if(id_dest->width!=4)
  {
    rtl_andi(&s0,&s0,(0xffffffffu>>((4-id_dest->width)*8)));
  }
  rtl_update_ZFSF(&s0,id_dest->width);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_not(&s0,&id_dest->val);
  operand_write(id_dest,&s0);
  print_asm_template1(not);
}

make_EHelper(rol)
{
  for(int i=0;i<id_src->val;i++)
  {
    rtl_msb(&s0,&id_dest->val,id_dest->width);
    id_dest->val = (id_dest->val<<1)+s0;
  }
  rtl_lsb(&s1,&id_dest->val);
  rtl_set_CF(&s1);
  //count == 1 , update OF
  if(id_src->val==1)
    {
      rtl_msb(&s0,&id_dest->val,id_dest->width);
      rtl_get_CF(&s1);
      rtl_xor(&s1,&s0,&s1);
      rtl_set_OF(&s1);
    }
    operand_write(id_dest,&id_dest->val);
    print_asm_template1(rol);
}

make_EHelper(rcl)
{
  for(int i=0;i<id_src->val;i++)
  {
    rtl_msb(&s0,&id_dest->val,id_dest->width);
    rtl_get_CF(&s1);
    id_dest->val = (id_dest->val<<1)+s1;
    rtl_set_CF(&s0);
  }
  //count == 1 , update OF
  if(id_src->val==1)
    {
      rtl_msb(&s0,&id_dest->val,id_dest->width);
      rtl_get_CF(&s1);
      rtl_xor(&s1,&s0,&s1);
      rtl_set_OF(&s1);
    }
    operand_write(id_dest,&id_dest->val);
    print_asm_template1(rcl);
}

make_EHelper(ror)
{
  for(int i=0;i<id_src->val;i++)
  {
    rtl_lsb(&s0,&id_dest->val);
    id_dest->val = (id_dest->val>>1)+(s0<<(id_dest->width*8-1));
  }
  rtl_msb(&s1,&id_dest->val,id_dest->width);
  rtl_set_CF(&s1);
  //count == 1 , update OF
  if(id_src->val==1)
    {
      rtl_msb(&s0,&id_dest->val,id_dest->width);
      s1 = (id_dest->val&(0x1<<(id_dest->val*8-2)))>>(id_dest->val*8-2);
      rtl_xor(&s1,&s0,&s1);
      rtl_set_OF(&s1);
    }
    operand_write(id_dest,&id_dest->val);
    print_asm_template1(ror);
}

make_EHelper(rcr)
{
  if(id_src->val==1)
  {
    rtl_msb(&s0,&id_dest->val,id_dest->width);
    rtl_get_CF(&s1);
    rtl_xor(&s1,&s0,&s1);
    rtl_set_OF(&s1);
  }
  for(int i=0;i<id_src->val;++i)
  {
    rtl_get_CF(&s0);
    rtl_lsb(&s1,&id_dest->val);
    id_dest->val =(id_dest->val>>1)+(s0<<(id_dest->width*8-1));
    rtl_set_CF(&s1);
  }
  operand_write(id_dest,&id_dest->val);
  print_asm_template1(rcr);
}