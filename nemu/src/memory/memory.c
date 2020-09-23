#include "nemu.h"
#include "device/map.h"

//猜测:这里是模拟器向物理主机申请128MB内存进行模拟操作
uint8_t pmem[PMEM_SIZE] PG_ALIGN = {};

static IOMap pmem_map = {
  .name = "pmem",
  .space = pmem,
  .callback = NULL
};

//用来记录物理内存的起始位置
//X86架构下物理内存的起始也就是0传入的base=0;但是在其他架构下物理内存的其实位置不同传入base也就不同

void register_pmem(paddr_t base) {
  pmem_map.low = base;
  pmem_map.high = base + PMEM_SIZE - 1;

  Log("Add '%s' at [0x%08x, 0x%08x]", pmem_map.name, pmem_map.low, pmem_map.high);
}

IOMap* fetch_mmio_map(paddr_t addr);

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  if (map_inside(&pmem_map, addr)) {
    uint32_t offset = addr - pmem_map.low;
    return *(uint32_t *)(pmem + offset) & (~0u >> ((4 - len) << 3));
  }
  else {
    return map_read(addr, len, fetch_mmio_map(addr));
  }
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
  if (map_inside(&pmem_map, addr)) {
    uint32_t offset = addr - pmem_map.low;
    memcpy(pmem + offset, &data, len);
  }
  else {
    return map_write(addr, data, len, fetch_mmio_map(addr));
  }
}
