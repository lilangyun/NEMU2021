#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"  //包含read_ModR_M()函数

#define instr mov

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

// 所有helper函数的定义模板
make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)
make_instr_helper(cr2r)  //将control-register的值传入gpr中
make_instr_helper(r2cr)  //将gpr中的值传入control-register中

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);  //这是取操作数，即译码
	MEM_W(addr, REG(R_EAX));  //内存写，即执行

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;  //这里返回指令长度
}  //这就是自己编译码函数+执行的操作

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

#if DATA_BYTE==4
make_helper(concat(mov_cr2r_, SUFFIX)) {
	op_src->val=cpu.cr0;
	op_dest->size = DATA_BYTE;
	int len = read_ModR_M(eip, op_dest, op_src2);  //这个是读取ModR/M字节的函数
	do_execute();
	print_asm("mov" str(SUFFIX) " %%%s,%%%s", "cr0", REG_NAME(op_dest->reg));
	return len;  //  /r：长度值为1
}

make_helper(concat(mov_r2cr_, SUFFIX)){
	op_src->size=DATA_BYTE;
	int len=read_ModR_M(eip,op_src,op_src2);
	cpu.cr0=op_src->val;
	print_asm("mov" str(SUFFIX) " %%%s,%%%s", REG_NAME(op_dest->reg),"cr0");
	return len;
}

#endif

#include "cpu/exec/template-end.h"
