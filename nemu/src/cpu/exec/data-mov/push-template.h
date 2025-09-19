#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    cpu.esp-=DATA_BYTE;
    // 我怎么知道如何读寄存器呢？试着找一下关于寄存器的指令
    //这里其实像那个mov指令，我们可以去看看mov指令做了什么
    // 关键在于我需要先识别出是需要在esp中写入哪个寄存器的值，这涉及到取值+译码
    // 此时我意识到为什么要先做call指令，因为直接写入cpu.eip的值才是最简单的
    op_dest->type = OP_TYPE_MEM;
	op_dest->addr=cpu.esp;
    op_dest->size=DATA_BYTE;
    OPERAND_W(op_dest, op_src->val);
    print_asm("push r32");//像jmp一样打印一下吧，表示调用函数的位置
}
// 我应该做一个make_helper在其内部实现do_execute就结束

make_helper(concat(push_r_, SUFFIX)) {
	concat(decode_r_, SUFFIX)(eip);
	do_execute();
	return 1;
}

#include "cpu/exec/template-end.h"