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
	// assert(0);
	op_src->size=DATA_BYTE;
	int len=read_ModR_M(cpu.eip+1,op_src,op_dest);
	// 此时op_dest->reg表示segment register的编码
	cpu.sreg[op_dest->reg].val=op_src->val;

	uint16_t index=cpu.sreg[op_dest->reg].INDEX;
	lnaddr_t descriptor_lnaddr=(cpu.GDTR.base+8*index);
	lnaddr_t d0=lnaddr_read(descriptor_lnaddr,4);  //描述符的低位
	lnaddr_t d1=lnaddr_read(descriptor_lnaddr+4,4);  //描述符的高位
	lnaddr_t base = (((d1>>24)&0xFF)<<24)|((d1&0xFF)<<16)|((d0>>16)&0xFFFF);
	lnaddr_t limit=(d0&0xFFFF)|(((d1>>16)&0x7)<<16);
	cpu.sreg[op_dest->reg].base=base;
	cpu.sreg[op_dest->reg].limit=limit;

	print_asm("movw 0x%x,%%sreg[%x]",op_src->val,op_dest->reg);
	
	return len+1;
}
#endif

#if DATA_BYTE==4
make_helper(mov_cr2r_l) {
	// assert(0);
	op_src->size=4;
	op_dest->size=4;
	int len = read_ModR_M(cpu.eip+2,op_dest,op_src);
	//这个是读取ModR/M字节的函数
	// op_src->reg=cpu.cr0.val
	if(op_src->reg==0){
		OPERAND_W(op_dest, cpu.cr0.val,1);
		print_asm("movl cr0,%%%s", REG_NAME(op_dest->reg));
	}
	else if(op_src->reg==3){
		OPERAND_W(op_dest, cpu.cr3.val,1);
		print_asm("movl cr3,%%%s", REG_NAME(op_dest->reg));
	}
	else{
		assert(0);
	}
	// print_asm("movl cr0,%%%s", REG_NAME(op_dest->reg));
	return len+1;  //  /r：长度值为1
}

make_helper(mov_r2cr_l){
	op_src->size=4;
	int len=read_ModR_M(cpu.eip+2,op_src,op_dest);
	// int len = decode_r_l(cpu.eip+1);
	if(op_dest->reg==0){
		cpu.cr0.val=op_src->val;
	}
	else if(op_dest->reg==3){
		cpu.cr3.val=op_src->val;
	}
	else{
		assert(0);
	}
	print_asm("movl %%%s,cr0", REG_NAME(op_dest->reg));
	return len+1;
}
#endif

#include "cpu/exec/template-end.h"
