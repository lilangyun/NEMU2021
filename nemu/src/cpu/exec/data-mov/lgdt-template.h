#include "cpu/exec/template-start.h"

// 直接编写decode+execute函数即可
#if DATA_BYTE == 2 || DATA_BYTE  == 4
make_helper(concat(lgdt_, SUFFIX)) {
    uint32_t addr=(eip+3,4);

    uint16_t lim=swaddr_read(addr,2);
    uint32_t base=swaddr_read(addr,4);
    // uint16_t lim=decode_rm_l(eip+3,2);
    // uint32_t base=instr_fetch(eip+5,4);
    cpu.GDTR.lim=lim;
    if(DATA_BYTE==2){
        cpu.GDTR.base=base&(0x0111);  //确保只提取低3字节数据
    }
    else{
        cpu.GDTR.base=base;
    }  //执行
	print_asm("lgdt" str(SUFFIX) " 0x%x", addr);
	if(DATA_BYTE==2){
        return 8;
    }
    return 9;
}
#endif

#include "cpu/exec/template-end.h"
