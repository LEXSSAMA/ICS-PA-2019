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
  return ramdisk_write(buf,offset,len);
}
size_t file_read(void *buf,size_t offset,size_t len){
  return ramdisk_read(buf,offset,len);
}
/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0,invalid_read, invalid_write},
  {"stdout", 0, 0, 0,invalid_read, invalid_write},
  {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))
//for debug
void printf_file_table(){
  for(int i=0;i<NR_FILES;i++){
    printf("name=%s,size=%d,disk_offset=%d,open_offset=%d\n",file_table[i].name,file_table[i].size,
            file_table[i].disk_offset,file_table[i].open_offset);
  }
}
void init_fs() {
  for (size_t i = 3; i < NR_FILES; i++)
  {
    file_table[i].open_offset=0;
    file_table[i].read = file_read;
    file_table[i].write = file_write;
  }
  // TODO: initialize the size of /dev/fb
}
int fs_open(const char* pathname,int flags,int mode){
  for(int i=0;i<NR_FILES;++i){
      if(strcmp(pathname,file_table[i].name)==0)
          return i;
  }
	panic("The pathname: [%s] don't exist!", pathname);
  return -1;
}
int fs_read(int fd, void *buf,int len){
  assert(fd<NR_FILES);
  assert(buf!=NULL);
  assert(file_table[fd].open_offset+len<file_table[fd].size);
  file_table[fd].read(buf,file_table[fd].open_offset+file_table[fd].disk_offset,len);
  file_table[fd].open_offset+=len;
  return len;
}
int fs_write(int fd,const void* buf,int len){
  if(fd!=FD_FB)
  return file_table[fd].write(buf,file_table[fd].open_offset+file_table[fd].disk_offset,len);
  assert(fd<NR_FILES);
  assert(buf!=NULL);
  assert(file_table[fd].open_offset+len<file_table[fd].size);
  file_table[fd].write(buf,file_table[fd].open_offset+file_table[fd].disk_offset,len);
  file_table[fd].open_offset+=len;
  return len;
}
int fs_lseek(int fd,int offset,int whence){
  assert(fd<NR_FILES);
  switch (whence)
  {
  case SEEK_CUR:
    assert(file_table[fd].open_offset+offset<file_table[fd].size);
    file_table[fd].open_offset+=offset;
    break;
  case SEEK_END:
    assert(offset<=0);
    file_table[fd].open_offset=file_table[fd].size+offset;
  case SEEK_SET:
    assert(offset<=file_table[fd].size);
    file_table[fd].open_offset=offset;
    break;
  default :
    panic("The whence %d don't exist!\n",whence);
    break;
  }
  return 0;
}
int fs_close(int fd){
  return 0;
}
