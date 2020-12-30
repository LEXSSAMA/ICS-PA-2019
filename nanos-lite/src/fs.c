#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};
int file_num = 0;

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  assert(buf!=NULL);
  size_t pri_len = 0;
  const char* p =(const char*)buf;
  while(*p!='\0'){
    _putc(*p++);
    pri_len++;
  }
  return pri_len;
}

size_t file_write(const void *buf,size_t offset,size_t len){
  return 0;
}
size_t file_read(void *buf,size_t offset,size_t len){
  return 0;
}
/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0,invalid_read, invalid_write},
  {"stdout", 0, 0, 0,invalid_read, invalid_write},
  {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  file_num = sizeof(file_table)/sizeof(Finfo);
  // TODO: initialize the size of /dev/fb
}
int fs_open(const char* pathname,int flags,int mode){
  for(int i=0;i<file_num;++i){
      if(strcmp(pathname,file_table[i].name)==0)
          return i;
  }
  return -1;
}
int fs_read(int fd, void *buf,int len){
  return 0;
}
int fs_write(int fd,const void* buf,int len){
  assert(fd<file_num&&buf!=NULL);
  return file_table[fd].write(buf,file_table[fd].open_offset,len);
}
int fs_lseek(int fd,int offset,int whence){
  return 0;
}
