#include "cpu/exec/helper.h"


/* for instruction encoding overloading */
make_helper(leave) {
    cpu.esp=cpu.ebp;
    cpu.ebp = swaddr_read(cpu.esp, 4,1);
    cpu.esp+=4;
	print_asm("leave");
	return 1;
}