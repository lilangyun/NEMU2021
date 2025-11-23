#include "cpu/exec/helper.h"


/* for instruction encoding overloading */
make_helper(cld) {
    cpu.eflags.DF=0;
	print_asm("cld $0,%%DF");
	return 1;
}