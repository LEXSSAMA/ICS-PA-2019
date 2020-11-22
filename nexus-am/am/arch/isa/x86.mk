#CFLAGS:
#-m32: Generate code for 32bit sets "int" , "long",and pointer types to 32 bits
# and generates code that runs on any i386 system.

#-fpic,-fno-pic : Enable and disable the generation of position independent code

# -fno-omit-frame-pointer
# The -fno-omit-frame-pointer option instructs the compiler to store the stack frame pointer in a register. 
# the register that is used as frame pointer is not available for use as general-purpose register

#-fomit-frame-pointer 
#Don't keep the frame pointer in a register for funtions that don't need one
# this avoids the instructions to save , setup , and restore frame pointers; it also make an extra register available in many function.
# it also makes debugging impossible on some machines

# -march=name 
# GCC uses name to determine what kind of instructions it can emit when generating assembly code.
CFLAGS  += -m32 -fno-pic -fno-omit-frame-pointer -march=i386

#ASFLAGS
# -m32 : Select the word size , either 32 bits or 64 bits
# About the "word" explanation we can see this answer : https://stackoverflow.com/a/5295973/11969892
# -fno-pic: Don't generate position-independent code (default)
ASFLAGS += -m32 -fno-pic

#-m emulation: Emulate the emulation linker , You can list the available emulations with --verbose or -V options
#-melf_i386 : Emulate the elf_i386 linker
LDFLAGS += -melf_i386
