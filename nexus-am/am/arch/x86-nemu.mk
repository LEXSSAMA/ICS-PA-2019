include $(AM_HOME)/am/arch/isa/x86.mk
# -mstringop-strategy=alg Override the internal decision heuristic for the particular algorithm to use for inlining string operations. The allowed values for alg are:
# loop : Expand into an inline loop
CFLAGS += -mstringop-strategy=loop
include $(AM_HOME)/am/arch/platform/nemu.mk
