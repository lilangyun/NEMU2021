#include "common.h"
#include "memory.h"
#include <string.h>
#include <elf.h>
#include <stdio.h>

#define ELF_OFFSET_IN_DISK 0

#ifdef HAS_DEVICE
void ide_read(uint8_t *, uint32_t, uint32_t);
#else
void ramdisk_read(uint8_t *, uint32_t, uint32_t);
#endif

#define STACK_SIZE (1 << 20)

void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader() {
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph = NULL;	//程序头表，指向物理内存为0的位置

	uint8_t buf[4096];	//缓冲区
	// 4096/4==1024
	// assert(0);

#ifdef HAS_DEVICE
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
#else
	assert(0);
	ramdisk_read(buf, ELF_OFFSET_IN_DISK, 4096);	//读出ramdisk中的内容
	// 读程序表头，但是这里之后竟然不执行，非常奇怪
#endif
	// assert(0);

	elf = (void*)buf;	//elf文件应从虚拟地址0处开始

	/* TODO: fix the magic number with the correct one */
	// 把这个魔数修改正确
	const uint32_t elf_magic = 0x464c457f;	//如何修改?ans:在elf.h文件中竟然有关于魔数的信息！神奇
	uint32_t *p_magic = (void *)buf;
	//按照个人理解这里需要判断elf_magic是否等于*buf，那怎么知道buf前4字节是什么呢？ 
	// assert(0);
	nemu_assert(*p_magic == elf_magic);

	/* Load each program segment */
	// panic("please implement me");
	int i=0;
	ph = (void*)(buf + elf->e_phoff);
	// assert(0);
	for(; i<elf->e_phnum; i++,ph++) {
		/* Scan the program header table, load each segment into memory */
		if(ph->p_type == PT_LOAD) {
    // 保存虚拟地址
    uint32_t va = ph->p_vaddr;
    
    // 获取对应的物理地址
    uint32_t pa = mm_malloc(va, ph->p_memsz);
    
    // 将物理地址转换为内核虚拟地址用于加载
    uint32_t kernel_va = (uint32_t)pa_to_va(pa);
    
    // 使用内核虚拟地址加载数据
    ramdisk_read((void*)kernel_va, ph->p_offset, ph->p_filesz);
    memset((void*)(kernel_va + ph->p_filesz), 0, ph->p_memsz - ph->p_filesz);

#ifdef IA32_PAGE
    /* Record the program break for future use. */
    extern uint32_t cur_brk, max_brk;
    uint32_t new_brk = va + ph->p_memsz - 1;  // 使用原始的虚拟地址
    if(cur_brk < new_brk) { max_brk = cur_brk = new_brk; }
#endif
}
	}

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);

#ifdef HAS_DEVICE
	create_video_mapping();
#endif
	write_cr3(get_ucr3());
#endif

	return entry;
}
