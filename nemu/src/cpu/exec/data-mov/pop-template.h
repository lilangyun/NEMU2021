#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    reg_l(op_src->reg) = MEM_R(cpu.esp,1);  //1表示栈段ss
	reg_l(R_ESP) += DATA_BYTE;
	print_asm_template1();
}
// 我应该做一个make_helper在其内部实现do_execute就结束

make_instr_helper(r);

#if DATA_BYTE == 2 || DATA_BYTE  == 4
make_helper(concat(pop_m_, SUFFIX)){
   	swaddr_t addr=instr_fetch(cpu.eip+1,DATA_BYTE);
   	uint32_t data=MEM_R(cpu.esp,1);
   	swaddr_write(addr,DATA_BYTE,data,2);
	print_asm("pop" str(SUFFIX) " 0x%x, 0x%x", data, addr);
	cpu.esp+=4;
	// assert(0);
	return 2+DATA_BYTE;
}
#endif

#include "cpu/exec/template-end.h"