#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  const char* c = s;
  size_t num =0;
  while(*c!='\0'){
    c++;
    num++;
  }
  return num;
}

char *strcpy(char* dst,const char* src) {
  memcpy(dst,src,strlen(src)+1);
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  return NULL;
}

char* strcat(char* dst, const char* src) {
      strcpy(dst+strlen(dst),src);
      return dst;
}

int strcmp(const char* s1, const char* s2) {
  const unsigned char * t1 = (const unsigned char*)s1;
  const unsigned char * t2 = (const unsigned char*)s2;
  unsigned char c1 , c2;
  do{
    c1 = (unsigned char)*t1++;
    c2 = (unsigned char)*t2++;
    if(c1 == '\0')
      return c1 - c2;
  }
  while(c1 == c2);
  return c1 - c2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  return 0;
}

void* memset(void* v,int c,size_t n) {
  if(v==NULL) return NULL;
  unsigned char* p = (unsigned char*)v;
  while(n){
   *p = c; 
    n--;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char* tout = (char*)out;
  const char* tin = (const char*)in;
  while(n>0){
    *tout++ = *tin++;
    n--;
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  return 0;
}

#endif
