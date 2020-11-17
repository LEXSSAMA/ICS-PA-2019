#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

static inline bool is_sign(const rtlreg_t* val,int width){
  return (*val>>(width*8-1))&0x1;
}
static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  reg_l(R_ESP) -=4;
  rtl_sm(&reg_l(R_ESP),src1,4);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  rtl_lm(dest,&reg_l(R_ESP),4);
  reg_l(R_ESP)+=4;
}

static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  //两个同符号的数相减不会产生溢出
  if(is_sign(src1,width)==is_sign(src2,width))
      *dest = 0;
  else
  {
    /*src1 is positive , positive-negative = negative,则代表溢出
      src1是负数 ,负数 - 正数 = 正数 ，则代表溢出*/
      rtl_sub(&t0,src1,src2);
      if(is_sign(&t0,width)!=is_sign(src1,width))
        *dest = 1;
      else
        *dest = 0;
  }
}

static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  if(*res>*src1)
    *dest = 1;
  else
    *dest = 0;
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  //如果两个数是符号相反的数就不会溢出
  if(is_sign(src1,width)!=is_sign(src2,width))
      *dest = 0;
  else
  {
    /*符号相同的情况下相加overflow的情况 : 正数+正数 = 负数, 负数+负数 = 正数 , 若得数与原来的数的符号不相同则发生溢出 */
    rtl_add(&t0,src1,src2);
    t1 = is_sign(&t0,width);
    if(t1!=is_sign(src1,width))
      *dest = 1;
    else
      *dest = 0;
  }
  
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  if(*res<*src1)
    *dest = 1;
  else
    *dest = 0;
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    reg_f(f)=*src; \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = reg_f(f); \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  t0 = *result;
  rtl_andi(&t0,&t0,(0xffffffffu>>((4-width)*8)));
  if(t0==0)
      t0 = 0;
  else
      t0 = 1;
  rtl_set_ZF(&t0);
  }
static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
   t0 = is_sign(result,width);
   rtl_set_SF(&t0);
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
