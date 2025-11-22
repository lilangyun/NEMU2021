#include "cpu/exec/helper.h"


/* for instruction encoding overloading */
make_helper(std) {
    cpu.eflags.DF=1;
	print_asm("std $1,%%DF");
	return 1;
}