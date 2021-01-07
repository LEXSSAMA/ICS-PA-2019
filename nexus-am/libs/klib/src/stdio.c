#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
void debug(char* str){
  while(*str!='\0'){
    _putc(*str++);
  }
}

int pasteNum (char* dest , uint32_t num ,int length , int base,int negflag){
  char* str = dest;
  int digitalLength = 0;
  do{
      uint32_t digital = num%base;
      num/=base;
      digitalLength++;
      if(digital>=10){
        *str++ = digital-10 + 'a';
      }else{
        *str++ = digital + '0';
      }
  }
  while(num!=0);

  if(negflag){
    *str++ = '-';
    digitalLength++;
  }

  while(digitalLength<length){
    *str++ = '0';
    digitalLength++;
  }
  *str = '\0';
  for(int i=0;i<digitalLength/2;++i){
    char tmp = dest[digitalLength-i-1];
    dest[digitalLength-i-1]=dest[i];
    dest[i]=tmp;
  }
  return digitalLength;
}

int pasteString(char* dest,char* src,int length){
  int src_length = strlen(src);
  while(src_length<length){
    *dest++ =' ';
    src_length++;
  }
  *dest='\0';
  strcat(dest,src);
  return src_length;
}

int pasteChar(char* dest,char src,int length){
    if(length<=1){
        *dest = src;
        return 1;
    }
    for(int i=0;i<length-1;++i){
      *dest++ = ' ';
    }
    *dest++ = src;
    return length;
}

int isDigital(const char* fmt)
{
  int num = *fmt-'0';
  if(num>=0&&num<=9)
    return 1;
  return 0;
}

int charToNum(const char* fmt) {
    return *fmt-'0';
}

void _sprintf_internal(char* dest,const char *fmt,va_list ap){
  char* str = dest;
  char character ;
  while(*fmt!='\0'){
    *str = '\0';
  int negflag = 0;
  int strLength = 0;
  int signedNum = 0;

  if(*fmt!='%'){
    *str++ = *fmt++;
    continue;
  }

  fmt++;

  while(isDigital(fmt)){
    strLength = strLength*10 + charToNum(fmt);
    fmt++;
  }
  switch (*fmt){
    case 'd' :
              signedNum = va_arg(ap,int);
              if(signedNum<0){
                signedNum = -signedNum;
                negflag = 1;
              }
              else
                negflag = 0;
              str += pasteNum(str,signedNum,strLength,10,negflag);
              break;
    case 'u' :
              str += pasteNum(str,va_arg(ap,uint32_t),strLength,10,0);
              break;
    case 'p' :
              *str++ = '0';
              *str++ = 'x';
              str += pasteNum(str,va_arg(ap,uint32_t),strLength,16,0);
              break;
    case 'x' :
              str += pasteNum(str,va_arg(ap,uint32_t),strLength,16,0);
              break;
    case 's' :
              str += pasteString(str,va_arg(ap,char*),strLength);
              break;
    case 'c' :
              character = (char)va_arg(ap,int);
              str += pasteChar(str,character,strLength);
              break;
    default: 
            break;
  }

  fmt++;
}
  *str = '\0';
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
