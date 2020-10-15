#include "nemu.h" 
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,TK_Plus , TK_EQ , TK_NOTEQ,TK_HexadecimalNumber,TK_AND,TK_Subtraction , TK_Asterisk
  ,TK_Diagonal , TK_LParentheses , TK_RParentheses
  ,TK_Decimal,TK_REG
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
  {"!=",TK_NOTEQ},
  {"&&",TK_AND},
  {"0[xX][0-9a-fA-F]+",TK_HexadecimalNumber},
  {"==", TK_EQ},         // equal
  {"-",TK_Subtraction},
  {"\\*",TK_Asterisk},
  {"/",TK_Diagonal},
  {"[a-zA-z]+",TK_REG}, //register
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

#define NR_TOKENS 200
static Token tokens[NR_TOKENS] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    if(nr_token>=NR_TOKENS)
    {
        printf("\033[0;33mThe expression too long !\n\033[0m;");
        return false;
    }
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
          case TK_HexadecimalNumber:
            tokens[nr_token].type=rules[i].token_type;
            if(substr_len>=32){
              printf("\033[0;33m Decimal Number Overflow !\n\033[0m;");
              return false;
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
    //判读不包括左右边缘的括号内的表达式是否合法
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
  //判读包括左右边缘的括号内的表达式是否合法
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

uint32_t findMainOp(int p,int q,bool* success)
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
            printf("Bad expression at [%d %d]\n",p,q);
           *success = false;
            return 0;
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
    {
      printf("Bad expression at [%d %d]\n",p,q);
      *success = false;
    }
    return op;
}
uint32_t calucate(int pos,bool* success)
{
  uint32_t total = 0;
      uint32_t i=0;
      bool isDecimal = tokens[pos].type==TK_Decimal ? true:false;
      while(tokens[pos].str[i]!='\0')
      {
        if(isDecimal)
        total=total*10+(tokens[pos].str[i++]-'0');
        else
        {
          if(i>=10)
           {
             printf("Bad hexadecimal number !\n");
             *success=false;
             return 0;
           }
          if(i==0||i==1)
          {
            i++;
            continue;
          }
            char c = tokens[pos].str[i++];
            if(c<='9'&&c>='0')
              total = total*16+(c-'0');
            else if (c<='f'&&c>='a')
              total = total*16+(c-'a'+10);
            else 
              total = total*16+(c-'A'+10);
       }
      }
      return total;
}
uint32_t eval(int p,int q,bool* success)
{
  if(p>q)
  {
      printf ("Bad expression ! p>q\n");
      *success=false;
      return 0;
  }
  else if(p==q)
  {
    if(tokens[p].type!=TK_Decimal&&tokens[p].type!=TK_HexadecimalNumber)
    {
      printf ("Bad expression !\n");
      *success=false;
      return 0;
    }
    
    return calucate(p,success);
  }
  int check = check_parentheses(p,q);
  if(check!=0)
  {
    if(check==-1)
    {
      printf("Bad expression ! check_parenthese\n");
      *success=false;
    }
    return eval(p+1,q-1,success);
  }
  else
  {
    uint32_t op = findMainOp(p,q,success);
    if(*success==false)
    return 0;
    uint32_t val1= eval(p,op-1,success);
    if(*success==false)
    return 0;
    uint32_t val2 = eval(op+1,q,success);
    if(*success==false)
    return 0;
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
        {
          printf("Divide by 0 !\n");
          *success=false;
          return 0;
        }
        return val1/val2;
      default: printf("Bad expression !\n"); *success = false;return 0;
    }
  }
}
void adjustment(int x , int cnt)
{
  for(int i=x;i+cnt<nr_token;++i)
  {
    tokens[i]=tokens[i+cnt];
  }
  nr_token-=cnt;
}

uint32_t expr(char *e, bool *success) {

  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for(int i=0;i<nr_token;++i)
  {
   if(tokens[i].type==TK_Plus||tokens[i].type==TK_Subtraction) 
   {
     int j=i;
     int flag =1;
     while (j<nr_token&&(tokens[j].type==TK_Plus||tokens[j].type==TK_Subtraction))
     {
          flag *= tokens[j].type==TK_Plus ? 1:-1;
          j++;
     }
    if(j-i>1) 
    {
      adjustment(i,j-i-1);
      tokens[i].type= flag==1? TK_Plus :TK_Subtraction;
    }
   }
  }
  bool remove_sign(int judgment)
  {
    return judgment==TK_Asterisk||judgment==TK_LParentheses||judgment==TK_Diagonal;
  }

  //把负数变成正数，通过+来实现-,如把-10变成4294967287
  //同时把(+/-)10(+/-)10,+10变换成10(+\-)10,10的形式,因为递归的写的函数expr()无法处理第一个加减符号
  for(int i=0;i<nr_token;++i)
  {
    if(tokens[i].type==TK_Subtraction)
    {
      if(i+1<nr_token&&tokens[i+1].type==TK_Decimal)
      {
          uint32_t count =0;
          int j=0;
          while (tokens[i+1].str[j]!='\0')
          {
            count = count*10+tokens[i+1].str[j++]-'0';
          }
          //由-变+
          count= -count;
          sprintf(tokens[i+1].str,"%u",count);
          //如果i==0就不用判断后面的了，如果i==0不成立那么i-1一定成立,(+100),*+100,/+100的情况全部变为(100),*100,/100的情况
          if(i==0||remove_sign(tokens[i].type))
            adjustment(i,1);
          else
            tokens[i].type=TK_Plus;
      }
    }
    else if(tokens[i].type==TK_Plus)
    {
      if(i+1<nr_token&&tokens[i+1].type==TK_Decimal)
    //如果i==0就不用判断后面的了，如果i==0不成立那么i-1一定成立,(+100),*+100,/+100的情况全部变为(100),*100,/100的情况
          if(i==0||remove_sign(tokens[i].type))
          adjustment(i,1);
    }
    // else if (tokens[i].type==TK_Asterisk&&(i==0||tokens[i-1].type!=TK_Decimal||tokens[i-1].type!=TK_HexadecimalNumber))
    //     {
  
    //     }
    }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  printf("\033[0;31m");
  uint32_t count = eval(0,nr_token-1,success);
  printf("\033[0m");
  return count;
}
