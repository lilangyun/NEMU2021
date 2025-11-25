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
	Elf32_Phdr *ph = NULL;	//程序头表，指向NEMU物理内存为0的位置
	// NULL和c语言的不一样，是NEMU中自定义的宏

	uint8_t buf[4096];	//缓冲区
	// 4096/4==1024
	// assert(0);

#ifdef HAS_DEVICE
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
#else
	// assert(0);
	ramdisk_read(buf, ELF_OFFSET_IN_DISK, 4096);	//读出ramdisk中的内容
	// 读程序表头，但是这里之后竟然不执行，非常奇怪
#endif

	elf = (void*)buf;	//elf文件应从虚拟地址0处开始

	/* TODO: fix the magic number with the correct one */
	// 把这个魔数修改正确
	const uint32_t elf_magic = 0x464c457f;	//如何修改?ans:在elf.h文件中竟然有关于魔数的信息！神奇
	uint32_t *p_magic = (void *)buf;
	//按照个人理解这里需要判断elf_magic是否等于*buf，那怎么知道buf前4字节是什么呢？ 
	// assert(0);
	nemu_assert(*p_magic != elf_magic);

	/* Load each program segment */
	// panic("please implement me");
	int i=0;
	ph = (void*)(buf + elf->e_phoff);
	// assert(0);
	for(; i<elf->e_phnum; i++,ph++) {
		/* Scan the program header table, load each segment into memory */
		if(ph->p_type==PT_LOAD) {
			ph->p_vaddr=mm_malloc(ph->p_vaddr, ph->p_memsz);  //为进程的一个段分配虚拟空间
			/* TODO: read the content of the segment from the ELF file 
			 * to the memory region [VirtAddr, VirtAddr + FileSiz)
			 
			 */
			// 从ELF文件的segment到内存区域读内容:即from起始虚拟地址to(起始虚拟地址+文件大小）
			ramdisk_read((void*)ph->p_vaddr, ph->p_offset, ph->p_filesz);	//读出ramdisk中的内容
			/* TODO: zero the memory region 
			 * [VirtAddr + FileSiz, VirtAddr + MemSiz)
			 */
			// 清空内存区域：即from（起始虚拟地址+文件大小）to（起始虚拟地址+内存大小）
			memset ((void *)(ph->p_vaddr+ph->p_filesz), 0, ph->p_memsz-ph->p_filesz);

#ifdef IA32_PAGE
			/* Record the program break for future use. */
			extern uint32_t cur_brk, max_brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
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
