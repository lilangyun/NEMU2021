#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute(){
	DATA_TYPE result=op_dest->val-op_src->val;
    update_eflags_pf_zf_sf(result);
    // we need to finish the judge of CF,AF,OF
    // CF:是否需要向高字节结尾或者向高字节进位（结合ds后的个人理解）
    cpu.eflags.CF=op_dest->val<op_src->val;
    // AF:我理解是低四位的cf
    cpu.eflags.AF=(op_dest->val&0xf)<(op_src->val&0xf);
    // OF:判断是否溢出[只需要判断是否在范围内是不是就行？显然无法成立]
    cpu.eflags.OF = (((op_dest->val ^ op_src->val) & (op_dest->val ^ result)) >> (DATA_BYTE * 8 - 1)) & 0x1;
    print_asm("cmp  %s,%s", op_src->str, op_dest->str);
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"