NAME = nemu
#如果$(MAKECMDGOALS)和clean）不相等则执行ifneq ...endif内的语句
#$MAKECMDGOALS是一个特殊参数:这个参数存放你在命令行指定的目标列表,如果什么都没指定则为空
ifneq ($(MAKECMDGOALS),clean) # ignore check for make clean
# ?=用来设置变量，当没有设置ISA值或者没有ISA这个变量的时候
ISA ?= x86
#相当于执行一条shell命令
ISAS = $(shell ls src/isa/)
#打印信息到标准输出
$(info Building $(ISA)-$(NAME))
#$(filter pattern…,text):
#Returns all whitespace-separated words in text that
#do match any of the pattern words, removing any words that do not match. 
ifeq ($(filter $(ISAS), $(ISA)), ) # ISA must be valid
#产生致命错误，并提示Invalid ISA. Supported: $(ISAS)给用户
$(error Invalid ISA. Supported: $(ISAS))
endif
endif

INC_DIR += ./include ./src/isa/$(ISA)/include
BUILD_DIR ?= ./build

#如果SHARE的值不为空就为true
ifdef SHARE
SO = -so
# -D_SHARE:-Dmacro=defn  相当于 C 语言中的 #define macro=defn
# -fPIC涉及到动态链接(先放一放)
SO_CFLAGS = -fPIC -D_SHARE=1
SO_LDLAGS = -shared -fPIC
endif

OBJ_DIR ?= $(BUILD_DIR)/obj-$(ISA)$(SO)
BINARY ?= $(BUILD_DIR)/$(ISA)-$(NAME)$(SO)

#类似C中的#include
include Makefile.git
#设置默认目标，如果没有在命令行指定目标则使用默认目标
.DEFAULT_GOAL = app

# Compilation flags
CC = gcc
LD = gcc
#$(addprefix,prefix,names...):The value of prefix is prepended to the front of 
# each individual name and the resulting larger names are concatenated with single
# spaces between them 
INCLUDES  = $(addprefix -I, $(INC_DIR))
# -O2:允许编译器对代码进行优化,级别为2
# -MMD:生成文件关联信息但是忽略由#include<file>造成的依赖关系并且写入filename.d文件中，可以去看看-M
# -Wall:开启所有警告信息
# -Werror: every warning is treated as an error
# -ggdb3:(搞不懂是什么意思)produces extra debugging information, for example: including macro definitions.
# -D__ISA__:-Dmacro=defn  相当于 C 语言中的 #define macro=defn
# -fomit-frame-pointer :(这个参数有关于栈指针)看这篇文章:https://stackoverflow.com/questions/14666665/trying-to-understand-gcc-option-fomit-frame-pointer
CFLAGS   += -O2 -MMD -Wall -Werror -ggdb3 $(INCLUDES) -D__ISA__=$(ISA) -fomit-frame-pointer

QEMU_DIFF_PATH = $(NEMU_HOME)/tools/qemu-diff
QEMU_SO = $(QEMU_DIFF_PATH)/build/$(ISA)-qemu-so

#执行make指令$(MAkE)是特殊变量 -C用来指定目录
$(QEMU_SO):
	$(MAKE) -C $(QEMU_DIFF_PATH)

# Files to be compiled
# -v 表示不匹配“isa"
SRCS = $(shell find src/ -name "*.c" | grep -v "isa")
SRCS += $(shell find src/isa/$(ISA) -name "*.c")
#$(var:a=b)，是将 var 变量中每一个单词后面的 a 替换为 b
OBJS = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)

# Compilation patterns
#@表示不显示执行的指令
#$<代表第一个依赖项
#$(dir NAMES...):取出每个文件名的目录部分
$(OBJ_DIR)/%.o: src/%.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(SO_CFLAGS) -c -o $@ $<


#看这篇文章https://blog.csdn.net/xiaozhi_su/article/details/4202779
# Depencies
#将OBJS中的文件后缀为.o的文件然后把后缀改为.d
-include $(OBJS:.o=.d)

# Some convenient rules

.PHONY: app run gdb clean run-env $(QEMU_SO)
app: $(BINARY)

override ARGS ?= -l $(BUILD_DIR)/nemu-log.txt
override ARGS += -d $(QEMU_SO)

# Command to execute NEMU
IMG :=
NEMU_EXEC := $(BINARY) $(ARGS) $(IMG)

$(BINARY): $(OBJS)
	$(call git_commit, "compile")
	@echo + LD $@
	@$(LD) -O2 -rdynamic $(SO_LDLAGS) -o $@ $^ -lSDL2 -lreadline -ldl

run-env: $(BINARY) $(QEMU_SO)

run: run-env
	$(call git_commit, "run")
	$(NEMU_EXEC)

gdb: run-env
	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

clean:
	-rm -rf $(BUILD_DIR)
	$(MAKE) -C tools/gen-expr clean
	$(MAKE) -C tools/qemu-diff clean
count:
	@echo  "\e[1;32m"
	@echo "The .c and .h file total number of row equal to :"
	@find ./ -name "*.[ch]" | xargs wc -l | awk 'END{printf "%s\n",$$1}'
	@echo "The .c and .h file (without blank line) total number of row equal to :"
	@find ./ -name "*.[ch]" | xargs cat | grep -v '^\s*$$'| wc -l
	@echo "\e[0m"

