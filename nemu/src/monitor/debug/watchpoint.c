#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32
static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp()
{
  if(free_==NULL)
    Assert(0,"\033[0;31mHave not free watchpoint!\n\033[0m");
  WP* tmp = free_;
  free_=free_->next;
  tmp->next=head;
  head=tmp;
  return head;
}

bool free_wp(int NO)
{
  if(NO>=NR_WP)
  {
    printf("\033[0;31mNumber 0f Watchpoint Over NR_WP!\n\033[0m");
    return 0;
  }
  WP* wNode = head;
  WP* preNode = NULL;
  while(wNode!=NULL)
  {
    if(wNode->NO!=NO) 
    {
      preNode=wNode;
      wNode=wNode->next;
      continue;
    }
    if(preNode!=NULL)
    {
      preNode->next=wNode->next;
      wNode->next=free_;
      free_=wNode;
    }
    else
    {
      head = wNode->next;
      wNode->next=free_;
      free_=wNode;
    }
    return true;
  }
 printf("\033[0;31mHave not number %d working watchpoint\n\033[0m",NO);
  return false;
}

bool check_watchpoints()
{
  WP* node = head; 
  bool result=true;
  while(node!=NULL)
  {
       bool success = true;
       uint32_t newValue = expr(node->expr,&success);
          if(!success)
          Assert(0,"\033[0;31mThe watchpoints expression got some touble.\n\033[0m");
          if(newValue!=node->oldValue)
          {
              printf("\033[0;33m");
              printf("watchpoints %d : %s\n",node->NO,node->expr);
              printf("Old Value : %d(0x%x)\n",node->oldValue,node->oldValue); 
              printf("New Value : %d(0x%x)\n",newValue,newValue); 
              printf("\033[0m");
              node->oldValue=newValue;
              result=false;
          }
          node=node->next;
  }
  return result;
}
void watchpoints_display()
{
  WP* node =head;
  printf("\033[0;34m");
  printf("NUM                    VALUE                    EXPR\n");
  while(node!=NULL)
  {
    uint32_t num = node->oldValue;
    int len=0;
    while(num>0)
    {
      num/=10;
      len++;
    }
    //(44-16-8-len)只是为了让输出来的数与上面的NUM VALUE EXPR对齐,没什么意义
    printf("%-16d%u(0x%-8x)%*s%s\n",
          node->NO,node->oldValue,node->oldValue,(44-16-8-len)," ",node->expr);
    node=node->next;
  }
  printf("\033[0m");
}
