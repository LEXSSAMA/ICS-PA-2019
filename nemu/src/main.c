#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);

uint32_t expr(char *e, bool *success);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
   FILE* fp = fopen("../tools/gen-expr/input","r");
   uint32_t num =0;
   bool success = false;
  for(int i=0;i<10000;++i)
  {
    char str[100];
    success=true;
    fscanf(fp,"%u %[^\n]",&num,str);
    int count = expr(str,&success);
    if(!success)
      printf("Bad expression : %s \n",str);
    if(count!=num)
      printf("The count from program : %u , The count from file : %u\n",count,num);
    printf("%s=%u\n",str,count);
  }
    fclose(fp);
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}


