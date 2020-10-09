#include "nemu.h" 
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,TK_Plus , TK_EQ , TK_Subtraction , TK_Asterisk
  ,TK_Diagonal , TK_LParentheses , TK_RParentheses
  ,TK_Decimal
  /* TODO: Add more token types */
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  /* 这里为什么匹配+号要使用\\+而不是\+呢？
  原因是即在正则表达式里用作转义字符，同时也在string（char*）字符里用作转义
  所以我们要创建一个\+正则表达式的时候，用一个char*字符串或者string字符串表示时应该
  写为\\+ */
  {"\\+", TK_Plus},         // plus
  {"==", TK_EQ},         // equal
  {"-",TK_Subtraction},
  {"\\*",TK_Asterisk},
  {"/",TK_Diagonal},
  {"\\(",TK_LParentheses},
  {"\\)",TK_RParentheses},
  {"[0-9]+",TK_Decimal}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[100] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE :
              break;
          case TK_Decimal :
          tokens[nr_token].type=rules[i].token_type;
          if(substr_len>=32){
            Assert(0,"Decimal Number Overflow !");
          }
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] ='\0';
            nr_token++;
              break;
          default: 
          tokens[nr_token].type = rules[i].token_type;
           nr_token++;
        }
        break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

/*Three status
* -1 : Bad expression!
*  0 : false, the whole expression is not surround pair of parentheses.
*  1 : true , the whole expression is surround pair of parentheses .
*/
int check_parentheses(int p,int q)
{
  int result = 0;
  int layer = 0;
  if(tokens[p].type==TK_LParentheses&&tokens[q].type==TK_RParentheses)
  {
    result=1;
    for(int i=p+1;i<=q-1;++i)
    {
      if(layer<0)
      {
        result=0;
      }
      switch (tokens[i].type)
      {
        case TK_LParentheses : layer++; break;
        case TK_RParentheses : layer--; break;
      }
    }
  }
  layer=0;
  for(int i=p;i<=q;++i)
  {
    if(layer<0)
    {
      result=-1;
      break;
    }
    switch (tokens[i].type)
      {
        case TK_LParentheses : layer++; break;
        case TK_RParentheses : layer--; break;
      }
  }
  return result;
}

uint32_t findMainOp(int p,int q)
{
    uint32_t op = p;
    int layer = 0;
    for(int i=p;i<=q;++i)
    {
      if(layer==0){
        switch(tokens[i].type)
        {
          case TK_Plus:
          case TK_Subtraction:
            op=i;
            break;
          case TK_Diagonal:
          case TK_Asterisk:
            if(tokens[op].type!=TK_Plus&&tokens[op].type!=TK_Subtraction)
              op=i;
            break;
          case TK_LParentheses:
             layer++;
            break;
          case TK_RParentheses:
            Assert(0,"Bad expression!");
            break;
        }
      }
      else
      {
        if(tokens[i].type==TK_RParentheses)
        layer--;
        else if(tokens[i].type==TK_LParentheses)
        layer++;
      }
    }
    if(layer!=0)
      Assert(0,"Bad expression !");
    return op;
}
uint32_t eval(int p,int q)
{
  if(p>q)
  {
    Assert(0,"p>q!");
  }
  else if(p==q)
  {
    if(tokens[p].type!=TK_Decimal)
    Assert(0,"tokens[p].type!=TK_Decimal");
    uint32_t total = 0;
    uint32_t i=0;
    while(tokens[p].str[i]!='\0')
    {
      total=total*10+(tokens[p].str[i++]-'0');
    }
    return total;
  }
  int check = check_parentheses(p,q);
  if(check!=0)
  {
    if(check==-1)
    Assert(0,"Bad expression !\n");
    return eval(p+1,q-1);
  }
  else
  {
    uint32_t op = findMainOp(p,q);
    //printf("p: %d,OP: %d,q: %d\n",p,op,q);
    uint32_t val1= eval(p,op-1);
    uint32_t val2 = eval(op+1,q);
    switch (tokens[op].type)
    {
      case TK_Plus:
        return val1+val2;
      case TK_Subtraction:
        return val1-val2;
      case TK_Asterisk:
        return val1*val2;
      case TK_Diagonal:
        if(val2==0)
        Assert(0,"Divide by 0 !\n");
        return val1/val2;
      default: Assert(0,"Bad expression !");
    }
  }
}

uint32_t expr(char *e, bool *success) {

  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  printf("%s\n",e);
  //printf("nr_tokens=%d\n",nr_token);
  // for(int i=0;i<nr_token;++i)
  // {
    // printf("%d: %s %d \n",i,tokens[i].str,tokens[i].type);
  // }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  int count = eval(0,nr_token-1);
  //printf("count = %d\n",count);

  return count;
}

