/*#include "rtl/rtl.h"

 Condition Code

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:
      rtl_get_OF(dest);
      break;
    case CC_B:
      rtl_get_CF(dest);
      break;
    case CC_E:
      rtl_get_ZF(dest);
        break;
    case CC_BE:
      rtl_get_CF(&s0);
      rtl_get_ZF(&s1);
      *dest = s0|s1;
      break;
    case CC_S:
      rtl_get_SF(dest);
      break;
    case CC_L:
      rtl_get_SF(&s0);
      rtl_get_OF(&s1);
     //if((reg_f(SF)==0&&reg_f(OF)==0)||(reg_f(SF)==1&&reg_f(OF)==1))
     //   *dest = 0;
     //else 
     //  *dest = 1;
     rtl_xor(dest,&s0,&s1);
        break;
    case CC_LE:
      printf("address: dest==%p s0==%p s1==%p\n",dest,&s0,&s1);
      rtl_get_ZF(dest);
      rtl_get_SF(&s0);
      rtl_get_OF(&s1);
      rtl_xor(&s0,&s0,&s1);
      rtl_or(dest,dest,&s0);
        break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
  assert(*dest == 0 || *dest == 1);
}*/

/*注释掉的这一段是我自己犯的一个大错误，因为我使用了传入参数dest来做中间变量，没有遵守就讲义中只使用s0和s1做中间变量的规定，错误的原因是
在做jcc跳转的时候调用rtl_setcc(),传入的dest参数正是s0,所以这里s0和参数dest其实是一样的，都指向同一片地址空间，所以当用dest=ZF,s0=SF时
dest记录的ZF就会被SF覆盖掉,所以造成了巨大的错误，引以为戒,遵守规则,下面是改正*/

#include "rtl/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:
      rtl_get_OF(&s1);
      break;
    case CC_B:
      rtl_get_CF(&s1);
      break;
    case CC_E:
      rtl_get_ZF(&s1);
        break;
    case CC_BE:
      rtl_get_CF(&s0);
      rtl_get_ZF(&s1);
      rtl_or(&s1,&s1,&s0);
      break;
    case CC_S:
      rtl_get_SF(&s1);
      break;
    case CC_L:
      rtl_get_SF(&s0);
      rtl_get_OF(&s1);
     //if((reg_f(SF)==0&&reg_f(OF)==0)||(reg_f(SF)==1&&reg_f(OF)==1))
     //   *dest = 0;
     //else 
     //  *dest = 1;
     rtl_xor(&s1,&s0,&s1);
        break;
    case CC_LE:
      rtl_get_SF(&s0);
      rtl_get_OF(&s1);
      rtl_xor(&s0,&s0,&s1);
      rtl_get_ZF(&s1);
      rtl_or(&s1,&s1,&s0);
        break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }
    *dest = s1;
  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
  assert(*dest == 0 || *dest == 1);
}