#include "cpu/exec/template-start.h"

#define instr cmp

make_helper(concat(cmp_i_a_, SUFFIX)) {
	concat(decode_i_a_, SUFFIX)(eip+1);
	uint32_t result=op_src2->val-op_src->val;
    update_eflags_pf_zf_sf(result);
    // we need to finish the judge of CF,AF,OF
    // CF:是否需要向高字节结尾或者向高字节进位（结合ds后的个人理解）
    cpu.eflags.CF=op_src2->val<op_src->val;
    // AF:我理解是低四位的cf
    cpu.eflags.AF=(op_src2->val&0x0000000f)<(op_src->val&0x0000000f);
    // OF:判断是否溢出[只需要判断是否在范围内是不是就行？显然无法成立]
    return 1+DATA_BYTE;

}
#include "cpu/exec/template-end.h"