#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"  //包含read_ModR_M()函数

#define instr mov

static void do_execute() {
	OPERAND_W(op_dest, op_src->val,1);
	print_asm_template2();
}

// 所有helper函数的定义模板
make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);  //这是取操作数，即译码
	MEM_W(addr, REG(R_EAX),2);  //内存写，即执行
	// 2表示ds

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;  //这里返回指令长度
}  //这就是自己编译码函数+执行的操作

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr,2);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

#if DATA_BYTE==2
// 设置寄存器
make_helper(mov_rm2sr_w){
	op_src->size=DATA_BYTE;
	int len=read_ModR_M(cpu.eip,op_src,op_dest);
	// 此时op_dest->reg表示segment register的编码
	cpu.sreg[op_dest->reg].val=op_src->val;

	uint16_t index=cpu.sreg[op_dest->reg].INDEX;
	lnaddr_t descriptor_lnaddr=cpu.GDTR.base+8*index;
	lnaddr_t d0=lnaddr_read(descriptor_lnaddr,4);  //描述符的低位
	lnaddr_t d1=lnaddr_read(descriptor_lnaddr+4,4);  //描述符的高位
	lnaddr_t base = ((d1>>8)&(0x1<<8)<<24)+((d1&(0x1<<8))<<16)+((d0>>16)&(0x1<<16));
	uint32_t limit=(d0&(0x1<<16))+((d1>>16)&(0x1<<3)<<16);
	cpu.sreg[op_dest->reg].base=base;
	cpu.sreg[op_dest->reg].limit=limit;
	
	return len;
}
#endif

#if DATA_BYTE==4
make_helper(mov_cr2r_l) {
	// assert(0);
	op_src->val=cpu.cr0.val;
	op_dest->size = 4;
	int len = read_ModR_M(cpu.eip+1, op_src2, op_dest);  //这个是读取ModR/M字节的函数
	do_execute();
	print_asm("mov" str(SUFFIX) " %%%s,%%%s", "cr0", REG_NAME(op_dest->reg));
	return len;  //  /r：长度值为1
}

make_helper(mov_r2cr_l){
	// assert(0);
	op_src->size=4;
	print_asm_template1();
	int len=read_ModR_M(cpu.eip+1,op_src2,op_src);
	cpu.cr0.val=op_src->val;
	print_asm("mov" str(SUFFIX) " %%%s,%%%s", REG_NAME(op_dest->reg),"cr0");
	return len;
}
#endif

#include "cpu/exec/template-end.h"
