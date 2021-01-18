#ifndef __PROC_H__
#define __PROC_H__

#include "common.h"
#include "memory.h"

#define STACK_SIZE (8 * PGSIZE)

typedef union {
  uint8_t stack[STACK_SIZE] PG_ALIGN;
  struct {
    _Context *cp;
    _AddressSpace as;
    // we do not free memory, so use `max_brk' to determine when to call _map()
    uintptr_t max_brk;
  };
} PCB;

extern PCB *current;

// Page directory and page table constants
#define PGSIZE         4096    // Bytes mapped by a page
#define PGSHFT         12      // log2(PGSIZE)
#define PTXSHFT        12      // Offset of PTX in a linear address
#define PDXSHFT        22      // Offset of PDX in a linear address

// Page table/directory entry flags
#define PTE_P          0x001   // Present

typedef uint32_t PTE;
typedef uint32_t PDE;
#define PDX(va)          (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)          (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va)          ((uint32_t)(va) & 0xfff)
#define ROUNDUP(a, sz)   ((((uintptr_t)a)+(sz)-1) & ~((sz)-1))
#define ROUNDDOWN(a, sz) ((((uintptr_t)a)) & ~((sz)-1))
#define PTE_ADDR(pte)    ((uint32_t)(pte) & ~0xfff)
#define PGADDR(d, t, o)  ((uint32_t)((d) << PDXSHFT | (t) << PTXSHFT | (o)))



#endif
