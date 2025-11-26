#include "cpu/exec/template-start.h"

#define instr jne

static void do_execute() {
    DATA_TYPE_S imm=op_src->val;
    if(cpu.eflags.ZF==0){
        cpu.eip+=imm;
    }
    print_asm_template1();
}
make_instr_helper(i)

#include "cpu/exec/template-end.h"