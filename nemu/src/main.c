#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);

uint32_t expr(char *e, bool *success);

void testExpr()
{
   FILE* fp = fopen("../tools/gen-expr/input","r");
   uint32_t num =0;
   bool success = false;
   char str[1000] ;
  while(fscanf(fp,"%u %[^\n]",&num,str)!=EOF)
  {
    success=true;
    int count = expr(str,&success);
    if(!success)
      printf("Bad expression : %s \n",str);
    if(count!=num)
      printf("The count from program : %u , The count from file : %u\n",count,num);
    printf("%s=%d\n",str,count);
  }
    fclose(fp);
    return ;
}

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
  //testExpr();
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}

