#include "nemu.h"
#include "memory/cache.h"

#define ENTRY_START 0x1000000

extern uint8_t entry [];
extern uint32_t entry_len;
extern char *exec_file;

void load_elf_tables(int, char *[]);
void init_regex();
void init_wp_pool();
void init_ddr3();

FILE *log_fp = NULL;

static void init_log() {
	log_fp = fopen("log.txt", "w");
	Assert(log_fp, "Can not open 'log.txt'");
}

static void welcome() {
	printf("Welcome to NEMU!\nThe executable is %s.\nFor help, type \"help\"\n",
			exec_file);
}

void init_monitor(int argc, char *argv[]) {
	/* Perform some global initialization */

	/* Open the log file. */
	init_log();

	/* Load the string table and symbol table from the ELF file for future use. */
	load_elf_tables(argc, argv);

	/* Compile the regular expressions. */
	init_regex();

	/* Initialize the watchpoint pool. */
	init_wp_pool();

	/* Display welcome message. */
	welcome();
}

#ifdef USE_RAMDISK
static void init_ramdisk() {
	int ret;
	const int ramdisk_max_size = 0xa0000;
	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	Assert(file_size < ramdisk_max_size, "file size(%zd) too large", file_size);

	fseek(fp, 0, SEEK_SET);
	ret = fread(hwa_to_va(0), file_size, 1, fp);
	assert(ret == 1);
	fclose(fp);
}
#endif

static void load_entry() {
	int ret;
	FILE *fp = fopen("entry", "rb");
	Assert(fp, "Can not open 'entry'");

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	ret = fread(hwa_to_va(ENTRY_START), file_size, 1, fp);
	assert(ret == 1);
	fclose(fp);
}

static void init_eflags(){
	cpu.eflags.val=0x2;  //这里我为了简洁，将0x00000002改为0x2
}

static void init_cr0(){  //static可确保外部文件不可调用本文件内部函数去修改状态等
	cpu.cr0.protect_enable=0;
	// 将PE置为0，确保初始时为实模式运行
	cpu.cr0.paging=0;
}

static void init_cs(){
	cpu.sreg[0].base=0x0;
	cpu.sreg[0].limit=0xffffffff;
}


void restart() {
	/* Perform some initialization to restart a program */
#ifdef USE_RAMDISK
	/* Read the file with name `argv[1]' into ramdisk. */
	// use main memory to imitate ramdisk(硬盘)
	init_ramdisk();
#endif

	/* Read the entry code into memory. */
	load_entry();

	/* Set the initial instruction pointer. */
	cpu.eip = ENTRY_START;

	/* Initialize DRAM. */
	init_ddr3();

	/*We need to initial the Eflags*/
	init_eflags();

	/*加载缓存*/
	init_cache();
	/*尝试在这里加载elf*/
	// load_elf_tables();//报错，失败

	/*Initialize the CR0 to assure we run in real-ilfe mode*/
	init_cr0();

	init_cs();

}
