#include "cpu/exec/template-start.h"

#define instr jns

static void do_execute() {
    if(cpu.eflags.SF==0){
        cpu.eip+=op_src->simm;
    }
    print_asm_template1();
}

make_instr_helper(si)
#include "cpu/exec/template-end.h"