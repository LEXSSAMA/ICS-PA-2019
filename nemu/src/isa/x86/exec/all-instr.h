#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

//arith.c
make_EHelper(add);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(sub);
make_EHelper(cmp);
make_EHelper(inc);
make_EHelper(dec);

//logic.c
make_EHelper(or);
make_EHelper(and);
make_EHelper(xor);

//data-mov.c
make_EHelper(push);
make_EHelper(pop);
make_EHelper(pusha);
make_EHelper(popa);