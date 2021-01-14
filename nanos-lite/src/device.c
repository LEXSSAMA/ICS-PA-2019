#include "common.h"
#include <amdev.h>

#ifndef KEYDOWN_MASK
#define KEYDOWN_MASK 0x8000
#endif

size_t serial_write(const void *buf, size_t offset, size_t len) {
  _yield();
  assert(buf!=NULL);
  const char* p =(const char*)buf;
  for(int i=0;i<len;++i){
    _putc(p[i]);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  _yield();
  assert(buf!=NULL);
  int key = read_key();
  if(key==_KEY_NONE){
    sprintf(buf,"t %u\n",uptime());
  }
  else{
    if(key&KEYDOWN_MASK){
      key ^= KEYDOWN_MASK;
      sprintf(buf,"kd %s\n",keyname[key]);
    }
    else{
      sprintf(buf,"ku %s\n",keyname[key]);
    }
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  if(strlen(dispinfo)<offset+len)
    len=strlen(dispinfo)-offset;
  memcpy(buf,dispinfo+offset,len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  _yield();
  offset/=4;
  int length = len/4;
  int screen_W = screen_width();
  int x = offset%screen_W;
  int y = offset/screen_W;
  int w = (length>(screen_W-x))?screen_W-x:length;
  int h = (length/w==0)?1:length/w;
  draw_rect((uint32_t*)buf,x,y,w,h);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo,"WIDTH:%u\nHEIGHT:%u\n",screen_width(),screen_height());
}
