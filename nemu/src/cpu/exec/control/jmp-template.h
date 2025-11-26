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
	// assert(0);
	swaddr_t addr=swaddr_read(cpu.eip+1,4,1);
	cpu.eip=addr-7;
	cpu.sreg[1].val=swaddr_read(cpu.eip+5,2,1);

	uint16_t index=cpu.sreg[1].val;  //这里val应该就是selector
	lnaddr_t descriptor_lnaddr=(cpu.GDTR.base+8*index);
	lnaddr_t d0=lnaddr_read(descriptor_lnaddr,4);  //描述符的低位
	lnaddr_t d1=lnaddr_read(descriptor_lnaddr+4,4);  //描述符的高位
	lnaddr_t base = (((d1>>24)&0xFF)<<24)|((d1&0xFF)<<16)|((d0>>16)&0xFFFF);
	lnaddr_t limit=(d0&0xFFFF)|(((d1>>16)&0x7)<<16);
	cpu.sreg[1].base=base;
	cpu.sreg[1].limit=limit;

	print_asm("ljmp 0x%x", addr);
	return 7;
}
#endif

#include "cpu/exec/template-end.h"
