#include "cpu/exec/helper.h"


/* for instruction encoding overloading */
make_helper(leave) {
    cpu.esp=cpu.ebp;
    cpu.ebp=cpu.esp;
    cpu.esp+=4;
	print_asm("leave");
	return 1;
}