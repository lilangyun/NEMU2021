#include "cpu/exec/template-start.h"

#define instr cmp

make_helper(concat(cmp_i2rm_, SUFFIX)) {
	int len=concat(decode_i2rm_, SUFFIX)(eip+1);
	uint32_t result=op_dest->val-op_src->val;
    update_eflags_pf_zf_sf(result);
    // we need to finish the judge of CF,AF,OF
    // CF:是否需要向高字节结尾或者向高字节进位（结合ds后的个人理解）
    cpu.eflags.CF=op_dest->val<op_src->val;
    // AF:我理解是低四位的cf
    cpu.eflags.AF=(op_dest->val&0x0000000f)<(op_src->val&0x0000000f);
    // OF:判断是否溢出[只需要判断是否在范围内是不是就行？显然无法成立]
    cpu.eflags.OF = (((op_dest->val ^ op_src->val) & (op_dest->val ^ result)) >> (DATA_BYTE * 8 - 1)) & 0x1;
    printf("cmp: op_dest=0x%x, op_src=0x%x, result=0x%x, ZF=%d\n", op_dest->val, op_src->val, result, cpu.eflags.ZF);
    return len+1;

}
#include "cpu/exec/template-end.h"