#include "cpu/exec/template-start.h"

#define instr test

make_helper(concat(je_si_, SUFFIX)) {
	concat(decode_si_, SUFFIX)(eip+1);
	bool flag=!!(cpu.eflags.val&0x02000000);
    if(flag){
        return 1+DATA_BYTE+op_src->val;
    }
	return 1+DATA_BYTE;
}
#include "cpu/exec/template-end.h"