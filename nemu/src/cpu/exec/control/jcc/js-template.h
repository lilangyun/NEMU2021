#include "cpu/exec/template-start.h"

#define instr js

static void do_execute() {
    if(cpu.eflags.SF==1){
        cpu.eip+=op_src->simm;
    }
    print_asm_template1();
}

make_instr_helper(si)
#include "cpu/exec/template-end.h"