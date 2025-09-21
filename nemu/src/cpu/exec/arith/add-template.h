#include "cpu/exec/template-start.h"

#define instr add

// 所有helper函数的定义模板

static void do_execute() {
	uint64_t result=(uint64_t)op_dest->val+(uint64_t)op_src->val;
	OPERAND_W(op_dest,(uint32_t)result);
    update_eflags_pf_zf_sf(result);
    // we need to finish the judge of CF,AF,OF
    // CF:是否需要向高字节结尾或者向高字节进位（结合ds后的个人理解）
    cpu.eflags.CF=(result>>(8*DATA_BYTE))&0x1;
    // AF:我理解是低四位的cf
    cpu.eflags.AF=((op_dest->val&0xf)+(op_src->val&0xf))>0xf;
    // OF:判断是否溢出[只需要判断是否在范围内是不是就行？显然无法成立]
    cpu.eflags.OF = MSB(~(op_dest->val ^ op_src->val) & (op_dest->val ^ result));
    print_asm_template2();
}

make_instr_helper(i2rm)

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"