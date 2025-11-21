#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"  //包含read_ModR_M()函数

// 直接编写decode+execute函数即可
#if DATA_BYTE == 2 || DATA_BYTE  == 4
make_helper(concat(lgdt_, SUFFIX)){
    op_src->size=4;
    int len=read_ModR_M(cpu.eip+1,op_src,op_src2);
    // assert(0);
    swaddr_t gdt_addr=op_src->addr;

    uint16_t lim=swaddr_read(gdt_addr,2,1);
    // assert(0);
    uint32_t base=swaddr_read(gdt_addr+2,4,1);  //取6位数据

    cpu.GDTR.lim=lim;
    #if DATA_BYTE==2
        cpu.GDTR.base=base&(0x0FFFFFF);  //确保只提取低3字节数据
    #else
        cpu.GDTR.base=base;
    #endif
	print_asm("lgdt" str(SUFFIX) " 0x%x", gdt_addr);
	return 1+len;
}
#endif

#include "cpu/exec/template-end.h"
