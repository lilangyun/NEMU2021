#include "cpu/exec/template-start.h"

#define instr push

// static void do_execute() {
//     cpu.esp-=DATA_BYTE;
//     // 我怎么知道如何读寄存器呢？试着找一下关于寄存器的指令
//     //这里其实像那个mov指令，我们可以去看看mov指令做了什么
//     // 关键在于我需要先识别出是需要在esp中写入哪个寄存器的值，这涉及到取值+译码
//     // 此时我意识到为什么要先做call指令，因为直接写入cpu.eip的值才是最简单的
//     MEM_W(cpu.esp,op_src->val);
// }
// // 我应该做一个make_helper在其内部实现do_execute就结束

// make_instr_helper(r);

static void do_execute () {
	reg_l (R_ESP) -= ((DATA_BYTE == 1) ? 4 : DATA_BYTE);
	if (DATA_BYTE == 1)op_src->val = (int8_t)op_src->val;
	swaddr_write(reg_l(R_ESP), 4, op_src->val);
	print_asm_template1();
}

make_instr_helper(i)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
make_instr_helper(rm)
#endif

#include "cpu/exec/template-end.h"