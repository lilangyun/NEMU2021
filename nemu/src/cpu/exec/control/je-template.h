#include "cpu/exec/template-start.h"

#define instr test

make_helper(concat(je_si_, SUFFIX)) {
	int len=concat(decode_si_, SUFFIX)(eip+1);
	bool flag=!!(cpu.eflags.val&0x00200000);
    if(flag){
        return len;
    }
	return 1+DATA_BYTE;
}
#include "cpu/exec/template-end.h"