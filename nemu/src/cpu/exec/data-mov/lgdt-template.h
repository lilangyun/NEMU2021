#include "cpu/exec/template-start.h"

// 直接编写decode+execute函数即可
#if DATA_BYTE == 2 || DATA_BYTE  == 4
make_helper(concat(lgdt_, SUFFIX)){
    uint32_t gd_addr=swaddr_read(cpu.eip+2,4,1);
    // assert(0);

    uint16_t lim=swaddr_read(gd_addr,2,1);
    uint32_t base=swaddr_read(gd_addr+2,4,1);  //取6位数据

    cpu.GDTR.lim=lim;
    #if DATA_BYTE==2
        cpu.GDTR.base=base&(0x0FFFFFF);  //确保只提取低3字节数据
    #else
        cpu.GDTR.base=base;
    #endif
	print_asm("lgdt" str(SUFFIX) " 0x%x", gd_addr);
	return 6;
}
#endif

#include "cpu/exec/template-end.h"
