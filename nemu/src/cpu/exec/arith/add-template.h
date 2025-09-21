#include "cpu/exec/template-start.h"

#define instr add

static void do_execute() {
	op_dest->val+=op_src->val;
	print_asm_template2();
}

// 所有helper函数的定义模板
make_instr_helper(i2rm)

#include "cpu/exec/template-end.h"