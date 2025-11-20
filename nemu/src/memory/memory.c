#include "common.h"
#include "memory/cache.h"
#include "../../../NEMU2021/nemu/include/cpu/reg.h"

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

lnaddr_t seg_translate(swaddr_t addr, uint8_t sreg){
	if(!cpu.cr0.protect_enable){
		return addr;
	}  //未进入保护模式
	uint16_t index=cpu.sreg[sreg].INDEX;
	lnaddr_t descriptor_lnaddr=(cpu.GDTR.base+8*index);
	lnaddr_t d0=lnaddr_read(descriptor_lnaddr,4);  //描述符的低位
	lnaddr_t d1=lnaddr_read(descriptor_lnaddr+4,4);  //描述符的高位
	lnaddr_t base = (((d1>>24)&0xFF)<<24)|((d1&0xFF)<<16)|((d0>>16)&0xFFFF);
	return base+addr;  //进入保护模式才需要地址翻译
}

uint32_t swaddr_read(swaddr_t addr, size_t len,uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);  //在未开启分段机制之前，虚拟地址可以直接看作是线性地址
	lnaddr_t lnaddr=seg_translate(addr,sreg);  //也就是添加将虚拟地址先转成线性地址
#endif
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
	lnaddr_t lnaddr=seg_translate(addr,sreg);
#endif
	lnaddr_write(lnaddr, len, data);
}
