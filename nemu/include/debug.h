#ifndef __DEBUG_H__
#define __DEBUG_H__
#include "common.h"
#include <stdio.h>
#include <assert.h>
#include "monitor/log.h"

#define Log(format, ...) \
    _Log("\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

//...表示可变参数
/*assert是一个宏，当存入的cond,当cond评估为ture的时候，assert()什么都不做,
但cond评估为false的时候，输出error信息到stderr并终止程序
fflush(stdout),强制刷新缓存区，将缓存区的内存输出到stdout中
__VA_ARGS__:代表可变参数(...)，详细可以看:https://stackoverflow.com/questions/26053959/what-does-va-args-in-a-macro-mean
 int fprintf(FILE *stream, const char *format, ...) sends formatted output to a stream*/
#define Assert(cond, ...) \
  do { \
    if (!(cond)) { \
      fflush(stdout); \
      fprintf(stderr, "\33[1;31m"); \
      fprintf(stderr, __VA_ARGS__); \
      fprintf(stderr, "\33[0m\n"); \
      assert(cond); \
    } \
  } while (0)

#define panic(...) Assert(0, __VA_ARGS__)

#define TODO() panic("please implement me")

#endif
