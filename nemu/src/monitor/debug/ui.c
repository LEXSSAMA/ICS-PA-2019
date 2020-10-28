#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display();
WP* new_wp();
bool free_wp(int NO);
void watchpoints_display();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;
  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}
static int cmd_si(char *args)
{
  char *arg = strtok(NULL," ");
  if(args==NULL)
  {
    cpu_exec(1);
  }
  else
  {
    int count =0;
    int len = strlen(arg);
    for(int i=0;i<len;++i)
    {
      count = count*10+(arg[i]-'0');
    }
    cpu_exec(count);
  }
  return 0;
}
static int cmd_info(char *args)
{
  char* str = strtok(NULL," ");
  if(str==NULL)
    return 0;
  if(strcasecmp(str,"r")==0)
  isa_reg_display(args);
  else if(strcasecmp(str,"w")==0)
  watchpoints_display();
  return 0;
}
static int cmd_p(char* args)
{
  bool success =true;
  uint32_t result = expr(args,&success);
  if(success)
    printf("\033[0;32m %s = %d(%#x) \033[0m;\n",args,result,result);
  return 0;
}
static int cmd_w(char *args)
{
 if(args==NULL)
  {
    printf("\033[1;33mPlease input the expression.\n\033[0m");
    return 0;
  }
  bool success = true;
  uint32_t oldValue = expr(args,&success);
  if(!success)
  {
    printf("\033[1;31mExpression Wrong!\n\033[0m");
    return 0;
  }
  WP* wp = new_wp();
  wp->oldValue=oldValue;
  memcpy(wp->expr,args,strlen(args)+1);
  printf("\033[0;32mwatchpoint %d : %s\n\033[0m",wp->NO,wp->expr);
  return 0;
}
static int cmd_d(char *args)
{
  char* str = strtok(NULL," ");
  bool success = true;
  int num = expr(str,&success);
  if(!success)
  {
    printf("\033[1;31mExpression Wrong!\n\033[0m");
    return 0;
  }
  free_wp(num);
  return 0;
}
static int cmd_x(char *args)
{
  unsigned int N = 0;
  unsigned int address = 0;
  bool success = true;
  char* arg = strtok(NULL," ");
  N = expr(arg,&success);
  if(!success)
  {
     printf("\033[1;31m please write correct expression ! \033[0m\n");
     return 0;
  }
  arg = strtok(NULL," ");
  address = expr(arg,&success);
  if(!success)
  {
     printf("\033[1;31m please write correct address ! \033[0m\n");
     return 0;
  }
    // if(address>=128*1024*1024)
    // {
    //     printf("\033[1;31m Memory address Overflow , Please write correct address!\033[0m\n");
    //    return 0;
    // }
    //0x100000和0x008100000一样;
    address=address%(128*1024*1024);
    printf("\033[1;32mAddress       Big-Endian     Little-Endian\033[0m\n");
    for(int i=0;i<N;++i)
    {
      char temp[80];
      snprintf(temp,20,"0x%08x%s%*s",address,":",3,"");
      for(int j=3;j>=0;--j)
      {
       char buff[80];
       snprintf(buff,20,"%02x ",pmem[address+j]);
       strcat(temp,buff);
      }
      strcat(temp,"   ");
      for(int j=0;j<4;++j)
      {
       char buff[80];
       snprintf(buff,20,"%02x ",pmem[address+j]);
       strcat(temp,buff);
      }
      printf("\033[1;32m%s\033[0m\n",temp);
      address+=4;
    }
  return 0;
}
static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","make program run [N] step, default is 1",cmd_si},
  { "info","info [SUBCMD] , info r : printf register status, info w : printf monitor pointer status",cmd_info},
  { "p","p [EXPR] : find the value of expression EXPR , for example : p $eax+1",cmd_p},
  { "x","x [N] [EXPR] : find the value of expression EXPR and use the result as the starting memory Address, output consecutive [N] 4 bytes in hexadecimal form. For example : x 10 0x100000",cmd_x},
  { "w","w [EXPR] : Stop run program When value of the EXPR change",cmd_w},
  { "d","d [N] : delete number [N] monitor pointer",cmd_d}
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("\033[1;32m%s - %s\033[0m\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("\033[1;32m%s - %s\033[0m\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }
  //开始解析传入的命令
  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);
    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

