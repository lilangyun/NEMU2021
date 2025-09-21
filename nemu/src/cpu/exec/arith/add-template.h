#include "cpu/exec/template-start.h"

#define instr add

// 所有helper函数的定义模板

make_helper(concat(add_i2rm_, SUFFIX)) {
	int len=concat(decode_i2rm_, SUFFIX)(eip+1);
	uint64_t result=(uint64_t)op_dest->val+(uint64_t)op_src->val;
	OPERAND_W(op_dest,result);
    update_eflags_pf_zf_sf(result);
    // we need to finish the judge of CF,AF,OF
    // CF:是否需要向高字节结尾或者向高字节进位（结合ds后的个人理解）
    cpu.eflags.CF=(result>>(8*DATA_BYTE))&0x1;
    // AF:我理解是低四位的cf
    cpu.eflags.AF=((op_dest->val&0xf)+(op_src->val&0xf))>0xf;
    // OF:判断是否溢出[只需要判断是否在范围内是不是就行？显然无法成立]
    cpu.eflags.OF = (((op_dest->val ^ op_src->val) & (op_dest->val ^ result)) >> (DATA_BYTE * 8 - 1)) & 0x1;
    print_asm_template2();
    return len+1;
}

#include "cpu/exec/template-end.h"