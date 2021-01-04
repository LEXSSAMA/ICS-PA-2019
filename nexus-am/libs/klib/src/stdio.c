#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
void reverse(char* dest,char* src){
   char* p = dest;
   int len = strlen(src);
   for(int i=len-1;i>=0;--i){
     *p++=src[i];
   }
   *p='\0';
   return;
}

void int_to_string(char* dest,int val){
  int len =0;
  char tmp[30];
  char* p = dest;
  if(val<0){
    *p++ = '-';
    val = -val;
  }else if(val==0){
    *p++='0';
  }
  while(val!=0){
   tmp[len++] =  (val%10+'0');
   val/=10;
  }
  tmp[len]='\0';
  *p = '\0';
  reverse(p,tmp);
  return ;
}

void uint_to_string(char* dest,uint32_t val){
  int len =0;
  char tmp[30];
  char* p = dest;
  if(val==0){
    *p++='0';
  }
  while(val!=0){
   tmp[len++] =  (val%10+'0');
   val/=10;
  }
  tmp[len]='\0';
  *p = '\0';
  reverse(p,tmp);
  return ;
}
int string_to_int(const char* str){
  int result =0;
  while(*str!='\0'){
    assert(*str>='0' && *str<='9');
    int num = *str-'0';
    result = result*10+num;
    str++;
  }
  return result;
}
char num_to_hex(int num){
  assert(num<16);
    if(num<10)
      return num+'0';
    else
      return (num-10)+'a';
}
void addr_to_hex_string(char* str,uint32_t addr){
  char tmp[30];
  int len =0;
  if(addr!=0){
    while(addr!=0){
      tmp[len++]=num_to_hex(addr%16);
      addr/=16;
    }
  }else{
    tmp[len++]='0';
  }
  tmp[len]='\0';
  reverse(str,tmp);
  return;
}
int padd(const char* fmt, char* padstr){
  int len =0;
  char* str = padstr;
  while(*fmt!='d'&&*fmt!='s'&&*fmt!='p'&&*fmt!='x'&&*fmt!='u'){
    *str++ = *fmt++;
    len++;
  }
  *str = '\0';
  return len;
}
int int_padd(char* dest,const char* padstr,const char* dig){
  char padd_char = *padstr=='0'?'0':' ';
  int total_len=0;
  int dig_len = strlen(dig);
  if(padd_char==' ')
   total_len= string_to_int(padstr);
  else
   total_len= string_to_int(padstr+1);
  int padd_time = total_len-dig_len;
  for(int i=0;i<padd_time;++i){
    *dest++ = padd_char;
  }
  *dest = '\0';
  return (padd_time>0?padd_time:0);
}
void _sprintf_internal(char* dest,const char *fmt,va_list ap){
    char str[256];
    char padstr[20];
    padstr[0]='\0';
    char *p = dest;
    while(*fmt!='\0'){
      *p='\0';
    if(*fmt!='%'){
      *p++ = *fmt++;
      continue;
    }
    fmt++;
    fmt+=padd(fmt,padstr);
    switch (*fmt)
    {
    case 'd':
    case 'u':
      if(*fmt=='d')
      int_to_string(str,va_arg(ap,int));
      else
      uint_to_string(str,va_arg(ap,uint32_t));
      if(strlen(padstr)!=0){
        p+=int_padd(p,padstr,str);
        padstr[0]='\0';
      }
      strcat(p,str);
      p+=strlen(str);
      break;
    case 's':
      strcat(str,va_arg(ap,char*));
      strcat(p,str);
      p+=strlen(str);
      break;
    case 'p':
    case 'x':
       addr_to_hex_string(str,va_arg(ap,uint32_t));
       strcat(p,str);
       p+=strlen(str);
       break;
    default:
      break;
    }
    fmt++;
    str[0]='\0';
}
  *p = '\0';
  return ;
}
int printf(const char *fmt, ...) {
  char buf[256];
  buf[0]='\0';
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
  int len = strlen(out);
  return len;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  if(n==0||n==1)
    return n;
  va_list arg;
  va_start(arg,fmt);
  _sprintf_internal(out,fmt,arg);
  out[n-1]='\0';
  va_end(arg);
  return n;
}

#endif
