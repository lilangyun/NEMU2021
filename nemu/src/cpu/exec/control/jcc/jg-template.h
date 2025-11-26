#include "cpu/exec/template-start.h"

#define instr jg

static void do_execute() {
    if(cpu.eflags.ZF==0&&(cpu.eflags.SF==cpu.eflags.OF)){
        cpu.eip+=op_src->imm;
    }
    print_asm_template1();
}

make_instr_helper(i)
#include "cpu/exec/template-end.h"