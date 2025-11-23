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

	print_asm("movw 0x%x,%%sreg[%x]",op_src->val,op_dest->reg);
	
	return len+1;
}
#endif

#if DATA_BYTE==4
make_helper(mov_cr2r_l) {
	uint8_t modrm=instr_fetch(cpu.eip+2,1);
	uint8_t mrm=modrm&0x7;  //rm类型为寄存器，这里储存的是寄存器编码
	uint8_t mreg=(modrm>>3)&0x7;  //这里reg指的是control register的编码
	uint8_t mop=(modrm>>6)&0x3;
	if (mop != 3) {
        assert(0);
    }
	switch (mreg)
	{
	case 0:{
		cpu.gpr[mrm]._32=cpu.cr0.val;
		break;
	}
	case 3:{
		cpu.gpr[mrm]._32=cpu.cr3.val;
		break;
	}
	default:{
		assert(0);
		break;
	}
	}
	print_asm("movl %%cr%d, %%%s",mreg,REG_NAME(mrm));
	return 2;
}

make_helper(mov_r2cr_l){
	uint8_t modrm=instr_fetch(cpu.eip+2,1);
	uint8_t mrm=modrm&0x7;  //rm类型为寄存器，这里储存的是寄存器编码
	uint8_t mreg=(modrm>>3)&0x7;  //这里reg指的是control register的编码
	uint8_t mop=(modrm>>6)&0x3;
	if (mop != 3) {
        assert(0);
    }
	switch (mreg)
	{
	case 0:{
		cpu.cr0.val=cpu.gpr[mrm]._32;
		break;
	}
	case 3:{
		cpu.cr3.val=cpu.gpr[mrm]._32;
		break;
	}
	default:{
		assert(0);
		break;
	}
	}
	print_asm("movl %%%s, %%cr%d",REG_NAME(mrm),mreg);
	return 2;
}
#endif

#include "cpu/exec/template-end.h"
