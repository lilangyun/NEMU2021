#include "cpu/exec/template-start.h"

// 直接编写decode+execute函数即可
#if DATA_BYTE == 2 || DATA_BYTE  == 4
make_helper(concat(lgdt_, SUFFIX)) {
    uint32_t lim=swaddr_read(cpu.eip+2,2,1);
    uint32_t base=swaddr_read(cpu.eip+4,4,1);  //取6位数据

    cpu.GDTR.lim=lim;
    #if DATA_BYTE==2
        cpu.GDTR.base=base&(0x0111);  //确保只提取低3字节数据
    #else
        cpu.GDTR.base=base;
    #endif
	print_asm("lgdt" str(SUFFIX) " 0x%x", eip);
	return 8;
}
#endif

#include "cpu/exec/template-end.h"
