#include "common.h"
#include "memory/cache.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);


/* Memory accessing interfaces */
// 访问内存的接口函数：sw(虚拟)，ln(线性)，hw(物理地址);一般用swaddr_访问模拟地址

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	uint8_t temp[2*block_size];
	// 判断是否跨越cache_block的边界
	uint32_t offset=addr&(block_size-1);
	cache_read(addr,temp);
	// 如果跨越
	if(offset+len>block_size){
		cache_read(addr+block_size,temp+block_size);
	}
	// 多么精妙
	return unalign_rw(temp + offset, 4)&(~0u>>((4-len)<<3));
	// return dram_read(addr, len) & (~0u >> ((4 - len) << 3));	//打包数据
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	cache_write(addr, data, len);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

