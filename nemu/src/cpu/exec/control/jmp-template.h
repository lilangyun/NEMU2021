#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	// 这里deepseek说是隐式更新
	cpu.eip += op_src->simm;
	// 这里显示跳转到的目标位置，无实际作用
	print_asm(str(instr) "  $0x%x", cpu.eip + 1 + DATA_BYTE);//+1是操作码;+databyte是立即数
}

make_instr_helper(si)
#if DATA_BYTE == 4
make_helper(jmp_rm_l) {
	int len = decode_rm_l(eip + 1);
	cpu.eip = op_src->val - (len + 1);
	print_asm(str(instr) " *%s", op_src->str);
	return len + 1;
}

make_helper(ljmp){
	swaddr_t addr=swaddr_read(cpu.eip+1,4,0);
	cpu.eip=swaddr_read(addr+2,4,1)-5;
	cpu.sreg[0].val=swaddr_read(addr,2,1);
	print_asm("ljmp *0x%x", addr);
	return 5;
}
#endif

#include "cpu/exec/template-end.h"
