#include "cpu/exec/template-start.h"

#define instr je

make_helper(concat(je_i_, SUFFIX)) {
	int len=concat(decode_i_, SUFFIX)(eip+1);
	bool flag=cpu.eflags.ZF;
    cpu.eip+=op_src->val;
    if(flag){
        print_asm(str(instr) " $0x%x", cpu.eip+1+len);
        return 1+len;
    }
	return 0;
}
#include "cpu/exec/template-end.h"