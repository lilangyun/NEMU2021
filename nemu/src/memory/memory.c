#include "common.h"
#include "memory/cache.h"
#include "cpu/reg.h"
#include "cpu/mmu.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#define pagesize 4096

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

// hwaddr_t page_translate(lnaddr_t addr){
// 	uint32_t offset=addr&0xFFF;
// 	uint32_t page=(addr>>12)&0x3FF;
// 	uint32_t dir=(addr>>22);

// 	// Log("CR3.page_directory_base=0x%x, dir_base=0x%x", 
//     //     cpu.cr3.page_directory_base, (cpu.cr3.page_directory_base << 12));

// 	hwaddr_t dir_base=(cpu.cr3.page_directory_base<<12);
// 	// hwaddr_t ptb_base=hwaddr_read(dir_base+dir*4,4);
// 	PDE pde;
// 	PTE pte;
// 	pde.val=hwaddr_read(dir_base+(dir<<2),4);
// 	// if(!pde.present){
// 	// 	assert(0);
// 	// }
// 	pte.val=hwaddr_read((pde.page_frame<<12)+(page<<2),4);
// 	// if(!pte.present){
// 	// 	assert(0);
// 	// }
// 	return (pte.page_frame<<12)+offset;
// }

hwaddr_t page_translate(lnaddr_t addr) {	// 线性地址 -> 物理地址
	if(!cpu.cr0.protect_enable || !cpu.cr0.paging) return addr;
	/* addr = 10 dictionary + 10 page + 12 offset */
	uint32_t dictionary = addr >> 22, page = (addr >> 12) & 0x3ff, offset = addr & 0xfff;
	/* 先看TLB */
	// int index = read_tlb(addr);
	// if (index != -1) return (tlb[index].data << 12) + offset;
	/* 读取页表信息 */
	uint32_t tmp = (cpu.cr3.page_directory_base << 12) + dictionary * 4;		// 页目录基地址 + 页目录号 * 页表项大小
	PTE dictionary_, page_;
	dictionary_.val = hwaddr_read(tmp, 4);
	tmp = (dictionary_.page_frame << 12) + page * 4;									// 二级页表基地址 + 页号 + 页表项大小
	page_.val = hwaddr_read(tmp, 4);
#ifdef DEBUG_page_p
	printf("eip:0x%x\taddr 0x%x\n", cpu.eip, addr);
#endif
	// Assert(dictionary_.present == 1, "dirctionary present != 1");
	// Assert(page_.present == 1, "second page table present != 1");
#ifdef DEBUG_page
	printf("0x%x\n", (page_.addr << 12) + offset);
#endif
	hwaddr_t addr_ = (page_.page_frame << 12) + offset;
	// write_tlb(addr, addr_);
	return addr_;
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	if(cpu.cr0.protect_enable&&cpu.cr0.paging){  //开启分页机制
		// assert(0);
		uint32_t offset=addr&(pagesize-1);
		if(offset+len>pagesize){  //if data cross boundrary
			assert(0);
		}
		else{
			hwaddr_t hwaddr=page_translate(addr);
			return hwaddr_read(hwaddr,len);
		}
	}
	// assert(0);
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	// assert(0);
	if(cpu.cr0.protect_enable&&cpu.cr0.paging){  //开启分页机制
		uint32_t offset=addr&(pagesize-1);
		// assert(0);
		if(offset+len>pagesize){  //if data cross boundrary
			assert(0);
		}
		else{
			hwaddr_t hwaddr=page_translate(addr);
			hwaddr_write(hwaddr,len,data);
		}
	}
	else{
		hwaddr_write(addr, len, data);
	}
}

lnaddr_t seg_translate(swaddr_t addr, uint8_t sreg){
	if(!cpu.cr0.protect_enable){
		// assert(0);
		return addr;
	}  //未进入保护模式
	// assert(0);
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
#endif
	lnaddr_t lnaddr=seg_translate(addr,sreg);  //也就是添加将虚拟地址先转成线性地址
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr=seg_translate(addr,sreg);
	lnaddr_write(lnaddr, len, data);
}
