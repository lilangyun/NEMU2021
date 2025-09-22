#include "cpu/exec/template-start.h"

#define instr lods

make_helper(concat(lods_, SUFFIX)) {
    // 第一步
	MEM_W(cpu.eax, MEM_R(cpu.esi));
    // 这里我直接粘贴movs代码，觉得差不多
	cpu.esi += (cpu.eflags.DF ? -DATA_BYTE : DATA_BYTE);
    // 最后一步打印
	print_asm("lods" str(SUFFIX) " %%ds:(%%esi),%%es:(%%edi)");
	return 1;
}

#include "cpu/exec/template-end.h"