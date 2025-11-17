#include "cpu/exec/template-start.h"

// 直接编写decode+execute函数即可
#if DATA_BYTE == 2 || DATA_BYTE  == 4
make_helper(concat(lgdt_, SUFFIX)) {
    uint16_t lim=instr_fetch(eip+3,2);
    uint32_t base=instr_fetch(eip+5,4);
    cpu.GDTR.lim=lim;
    if(DATA_BYTE==2){
        cpu.GDTR.base=base&(0x1110);
    }
    else{
        cpu.GDTR.base=base;
    }  //执行
	// print_asm("lgdt" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 9;
}
#endif

#include "cpu/exec/template-end.h"
