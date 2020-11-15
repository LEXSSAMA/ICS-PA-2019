#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

//arith.c
make_EHelper(add);
make_EHelper(adc);
make_EHelper(sbb);

//logic.c
make_EHelper(or);
make_EHelper(and);