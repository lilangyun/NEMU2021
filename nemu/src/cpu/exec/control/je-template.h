#include "cpu/exec/template-start.h"

#define instr je

make_helper(concat(je_i_, SUFFIX)) {
	int len=concat(decode_i_, SUFFIX)(eip+1);
	bool flag=cpu.eflags.ZF;
    cpu.eip+=1+len;
    if(flag){
        return op_src->val;
        print_asm(str(instr) " %x", cpu.eip);
    }
	return 0;
}
#include "cpu/exec/template-end.h"