#include "cpu/exec/template-start.h"

#define instr test

// 通用执行过程
static void do_execute() {
    
}
// 译码函数
make_instr_helper(rm2r)
#include "cpu/exec/template-end.h"