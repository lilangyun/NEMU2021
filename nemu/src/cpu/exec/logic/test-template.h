#include "cpu/exec/template-start.h"

#define instr test

make_helper(concat(test_rm_r_, SUFFIX)) {
	int len=concat(decode_rm_r_, SUFFIX)(eip+1);
	uint32_t temp=op_src->val&op_src2->val;
	cpu.eflags.CF=0;
    cpu.eflags.DF=0;
    update_eflags_pf_zf_sf(temp);
	print_asm("test %s, %s", op_src->str, op_src2->str);
	return len+1;
}
#include "cpu/exec/template-end.h"