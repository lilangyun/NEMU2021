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
	// 现在可以笃定就是ramdisk这里出现了问题
#endif

	elf = (void*)buf;	//elf文件应从虚拟地址0处开始

	/* TODO: fix the magic number with the correct one */
	// 把这个魔数修改正确
	const uint32_t elf_magic = 0x464c457f;	//如何修改?ans:在elf.h文件中竟然有关于魔数的信息！神奇
	uint32_t *p_magic = (void *)buf;
	//按照个人理解这里需要判断elf_magic是否等于*buf，那怎么知道buf前4字节是什么呢？ 
	nemu_assert(*p_magic == elf_magic);

	/* Load each program segment */
	// panic("please implement me");
	int i;
	ph = (void*)(buf + elf->e_phoff);
	// assert(0);
	for (i = 0; i < elf->e_phnum; i++, ph++) {
		if (ph->p_type != PT_LOAD) continue;

		// 为 [p_vaddr, p_vaddr + p_memsz) 建立页映射（不要改写 p_vaddr 本身）
		mm_malloc(ph->p_vaddr, ph->p_memsz);

		// 安全校验
		nemu_assert(ph->p_memsz >= ph->p_filesz);

#ifdef HAS_DEVICE
		ide_read((void *)ph->p_vaddr,
				 ph->p_offset + ELF_OFFSET_IN_DISK,
				 ph->p_filesz);
#else
		ramdisk_read((void *)ph->p_vaddr,
				 ph->p_offset,
				 ph->p_filesz);
#endif

		// 置零 BSS 部分
		if (ph->p_memsz > ph->p_filesz) {
			memset((void *)(ph->p_vaddr + ph->p_filesz), 0,
				 ph->p_memsz - ph->p_filesz);
		}

#ifdef IA32_PAGE
		extern uint32_t cur_brk, max_brk;
		uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
		if (cur_brk < new_brk) { max_brk = cur_brk = new_brk; }
#endif
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
