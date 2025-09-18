#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    cpu.esp-=4;
    swaddr_write(cpu.esp,4,cpu.eip);
    // 关键在于我需要先识别出是需要在esp中写入哪个寄存器的值，这涉及到取值+译码
    // 此时我意识到为什么要先做call指令，因为直接写入cpu.eip的值才是最简单的

}

make_instr_helper(si)

#include "cpu/exec/template-end.h"