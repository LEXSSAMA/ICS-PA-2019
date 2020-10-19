#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32
#define typename(x) _Generic((x),\
  _Bool:"bool", unsigned char: "unsigned char"\
  char:"char", signed char:"signed char"\
  short int:"short int", unsigned short int:"unsigned short int"\
  int:"int", unsigned int:"unsigned int"\
  long int: "long int", unsigned long int:"unsigned long int"\
  long long int:"long long int", unsigned long long int:"unsigned long long int"\
  float: "float",double:"double"\
  long double:"long double",char*:"pointer to char"\
  void*:"pointer to void",int*:"pointer to int")
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
  if(free_->next==NULL)
    Assert(0,"\033[0;31mHave not free watchpoint!\n\033[0m");
  WP* tmp = free_->next;
  free_->next=free_->next->next;
  return tmp;
}

void free_wp(WP* wp)
{
  if(wp->NO>=NR_WP)
    Assert(0,"\033[0;31mNumber 0f Watchpoint Over NR_WP!\n\033[0m");
  WP* wNode = head->next;
  while(wNode!=NULL)
  {
    if(wNode->next->NO!=wp->NO) 
    {
      wNode=wNode->next;
      continue;
    }
    WP* tmp  = wNode->next;
    wNode->next=wNode->next->next;
    tmp->next=head->next;
    head->next=tmp;
    return;
  }
  Assert(0,"\033[0;31mHave not number %d working watchpoint\n\033[0m",wp->NO);
  return ;
}
void check_watchpoints()
{

}
