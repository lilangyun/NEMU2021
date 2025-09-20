#include "cpu/exec/template-start.h"

#define instr je

make_helper(concat(je_i_, SUFFIX)) {
	int len=concat(decode_i_, SUFFIX)(eip+1);
	bool flag=cpu.eflags.ZF;
    if(flag){
        return 1+len+op_src->val;
    }
	return 1+len;
}
#include "cpu/exec/template-end.h"