#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    reg_l (R_ESP) -= ((DATA_BYTE == 1) ? 4 : DATA_BYTE);
	if (DATA_BYTE == 1)op_src->val = (int8_t)op_src->val;
	swaddr_write(reg_l(R_ESP), 4, op_src->val);
    print_asm("push %s",op_src->str);
}
// 我应该做一个make_helper在其内部实现do_execute就结束

make_instr_helper(r);
make_instr_helper(rm);

#include "cpu/exec/template-end.h"