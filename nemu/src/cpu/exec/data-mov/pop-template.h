#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    reg_l(op_src->reg) = MEM_R(cpu.esp,1);  //1表示栈段ss
	reg_l(R_ESP) += DATA_BYTE;
	print_asm_template1();
}
// 我应该做一个make_helper在其内部实现do_execute就结束

make_instr_helper(r);

#include "cpu/exec/template-end.h"