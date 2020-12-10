#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
void int_to_string(char* dest,int val){
  int len =0;
  char* p =dest;
  char tmp[30];
  if(val<0){
    *p++ = '-';
    val = -val;
  }
  while(val!=0){
   tmp[len++] =  (val%10+'0');
   val/=10;
  }
  for(int i=len-1;i>=0;--i){ 
    *p++ = tmp[i];
  }
  *p = '\0';
  return ;
}
void _sprintf_internal(char* dest,const char *fmt,va_list ap){
    char str[256];
    char *p = dest;
    while(*fmt!='\0'){
    if(*fmt!='%'){
      *p++ = *fmt++;
      continue;
    }
    fmt++;
    switch (*fmt)
    {
    case 'd':
      *p = '\0';
      int_to_string(str,va_arg(ap,int));
      strcat(p,str);
      p+=strlen(str);
      break;
    case 's':
      *p = '\0';
      strcat(str,va_arg(ap,char*));
      strcat(p,str);
      p+=strlen(str);
      break;
    default:
      break;
    }
    fmt++;
}
  *p = '\0';
  return ;
}
int printf(const char *fmt, ...) {
  char buf[256];
  va_list arg;
  va_start(arg,fmt);
  _sprintf_internal(buf,fmt,arg);
  va_end(arg);
  int len =strlen(buf);
  for(int i=0;i<len;++i){
    _putc(buf[i]);
  }
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list  arg ;
  va_start(arg,fmt);
  _sprintf_internal(out,fmt,arg);
  va_end(arg);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
