#include "nemu.h"
#include "monitor/monitor.h"
#include <unistd.h>

void init_log(const char *log_file);
void init_isa();
void init_regex();
void init_wp_pool();
void init_device();
void init_difftest(char *ref_so_file, long img_size);

static char *mainargs = "";
static char *log_file =NULL;
static char *diff_so_file = "";
static char *img_file = NULL;
static int is_batch_mode = false;

static inline void welcome() {
#ifdef DEBUG
  Log("Debug: \33[1;32m%s\33[0m", "ON");
  Log("If debug mode is on, A log file will be generated to record every instruction NEMU executes. "
      "This may lead to a large log file. "
      "If it is not necessary, you can turn it off in include/common.h.");
#else
  Log("Debug: \33[1;32m%s\33[0m", "OFF");
#endif

  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to \33[1;41m\33[1;33m%s\33[0m-NEMU!\n", str(__ISA__));
  printf("For help, type \"help\"\n");
}

static inline long load_img() {
  long size;
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
	/*声明外部文件的全局变量,
	 * isa_default_img[],isa_default_img_size
	 * 被定义在isa/$ISA/init.c
	 */
    extern uint8_t isa_default_img[];
    extern long isa_default_img_size;
    size = isa_default_img_size;
	/*void *memcpy(void *dest, const void *src, size_t n)
      copies n characters from memory area src to memory area dest.
      把isa_default_img数组中的机器码读入内存guest_to_host(IMAGE_START),
      也就是pmem+0x100000中*/
    memcpy(guest_to_host(IMAGE_START), isa_default_img, size);
  }
  else {
    int ret;
    /*FILE *fopen(const char *filename, const char *mode)
      opens the filename pointed to, by filename using the given mode.
      filename − This is the C string containing the name of the file to be opened
      mode − This is the C string containing a file access mode. 
      r:代表read,b代表二进制,r加上b表示已二进制的形式读*/
    FILE *fp = fopen(img_file, "rb");
    /*如果fp==NULL,Assert就会报错并终止程序*/ Assert(fp, "Can not open '%s'", img_file);

    Log("The image is %s", img_file);
	/*设置文件流的(写和读)开始位置,这里表示设置文件fp开始写和读的位置是seek_end+0(End of file + offset)*/
    fseek(fp, 0, SEEK_END);
	/* long int ftell(FILE *stream) returns the current file position of the given stream.
	返回文件位置指针相对与文件首的偏移字节数(猜测：可以看位文件光标所在的位置)*/
    size = ftell(fp);
	/*SEEK_SET(Beginning of file*/
    fseek(fp, 0, SEEK_SET);
	/*将fp文件里的1个大小为size的内容读入内存guest_to_host(IMAGE_START)的位置 成功读取的话就返回一个size_t 对象，数值等于我们想要读取size大小文件的个数(这里是等于1),如果不等于1,说明出现了问题*/
    ret = fread(guest_to_host(IMAGE_START), size, 1, fp);
	/*如果ret==1不等于true的话抛出错误，程序终止*/
    assert(ret == 1);
    /* int fclose(FILE *stream) closes the stream. All buffers are flushed.*/
    fclose(fp);

    // mainargs
   /*将mainargs指针指向内存的内容copy到guest_to_host(0)指针指向的内存*/
    strcpy(guest_to_host(0), mainargs);
  }
  return size;
}

static inline void parse_args(int argc, char *argv[]) {
  int o;
/*argc表示参数的个数，argv是一个数组里面装的是调用main函数的时候传递给参数
"-bl:d:a:"这里第一个字符设置为-表示当传入参数中不是b,l,d,a的时候会return1,这里传入参数-b后面可以不加参数,其他的-l,-d,-a都需要必须加参数,原因是l,d,a后面都有冒号,如果是c::两个冒号则表示-c后面可以加参数可以不加参数*/
  while ( (o = getopt(argc, argv, "-bl:d:a:")) != -1) {
    switch (o) {
      case 'b': is_batch_mode = true; break;
      case 'a': mainargs = optarg; break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 1:
                if (img_file != NULL) Log("too much argument '%s', ignored", optarg);
                else img_file = optarg;
                break;
      default:
                panic("Usage: %s [-b] [-l log_file] [img_file]", argv[0]);
    }
  }
}

int init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Open the log file. */
  init_log(log_file);

  /* Load the image to memory. */
  long img_size = load_img();

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();

  /* Initialize devices. */
  init_device();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size);

  /* Display welcome message. */
  welcome();

  return is_batch_mode;
}
