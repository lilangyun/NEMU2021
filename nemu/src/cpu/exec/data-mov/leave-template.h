#include "cpu/exec/template-start.h"

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(leave_, SUFFIX)) {
    cpu.esp=cpu.ebp;
    cpu.ebp=cpu.esp;
    cpu.esp+=DATA_BYTE;
	print_asm("leave" str(SUFFIX));
	return 1;
}
#endif


#include "cpu/exec/template-end.h"