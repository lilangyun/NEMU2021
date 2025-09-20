#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	DATA_TYPE result = op_dest->val - op_src->val;
  update_eflags_pf_zf_sf((DATA_TYPE_S)result);
	cpu.eflags.CF = op_dest->val < op_src->val;
  cpu.eflags.OF = MSB((op_dest -> val ^ op_src -> val) & (~(op_src -> val ^ result)));
    printf("cmp: op_dest=0x%x, op_src=0x%x, result=0x%x, ZF=%d\n", op_dest->val, op_src->val, result, cpu.eflags.ZF);
  print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)
#include "cpu/exec/template-end.h"