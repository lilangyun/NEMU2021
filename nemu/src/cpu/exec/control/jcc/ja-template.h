#include "cpu/exec/template-start.h"

#define instr ja

static void do_execute() {
	// 这里deepseek说是隐式更新
    DATA_TYPE_S imm=op_src->val;
    if(cpu.eflags.ZF==0&&cpu.eflags.CF==0){
        cpu.eip += imm;
    }
	// 这里显示跳转到的目标位置，无实际作用
	print_asm(str(instr) "  $0x%x", cpu.eip + 1 + DATA_BYTE);//+1是操作码;+databyte是立即数
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"