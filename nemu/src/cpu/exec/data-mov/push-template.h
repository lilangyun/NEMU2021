#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    cpu.esp-=4;
    // 我怎么知道如何读寄存器呢？试着找一下关于寄存器的指令
    swaddr_write(cpu.esp,DATA_BYTE,op_src->reg);
    // 关键在于我需要先识别出是需要在esp中写入哪个寄存器的值，这涉及到取值+译码
    // 此时我意识到为什么要先做call指令，因为直接写入cpu.eip的值才是最简单的
    print_asm("push %x", op_src->reg);//像jmp一样打印一下吧，表示调用函数的位置
}

make_instr_helper(r)

#include "cpu/exec/template-end.h"