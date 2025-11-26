#include "cpu/exec/helper.h"

make_helper(ret){
    print_asm("ret");
    cpu.eip+=1;
    cpu.eip=swaddr_read(cpu.esp,4,2)-1;
    cpu.esp+=4;
    return 1;
}

make_helper(ret_i_w){
    cpu.eip=swaddr_read(cpu.esp,4,2)-3;
    cpu.esp+=4+(uint16_t)decode_i_w(cpu.esp+1);
    print_asm_template1();
    return 3;
}
