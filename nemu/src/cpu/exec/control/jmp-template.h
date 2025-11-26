#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	// 这里deepseek说是隐式更新
	DATA_TYPE_S imm=op_src->val;
	cpu.eip += imm;
	// 这里显示跳转到的目标位置，无实际作用
	print_asm(str(instr) "  $0x%x", cpu.eip + 1 + DATA_BYTE);//+1是操作码;+databyte是立即数
}

make_instr_helper(i)
#if DATA_BYTE == 4
make_helper(jmp_rm_l) {
	int len = decode_rm_l(eip + 1);
	cpu.eip = op_src->val - (len + 1);
	print_asm(str(instr) " *%s", op_src->str);
	return len + 1;
}

make_helper(ljmp) {
    // 读取目标偏移地址和段选择子
    uint32_t offset = swaddr_read(cpu.eip + 1, 4, 1);
    uint16_t selector = swaddr_read(cpu.eip + 5, 2, 1);
    
    // 更新段选择子
    cpu.sreg[1].val = selector;
    
    // 计算描述符地址
    uint32_t index = selector >> 3;
    lnaddr_t descriptor_addr = cpu.GDTR.base + index * 8;
    
    // 读取描述符
    uint32_t d0 = lnaddr_read(descriptor_addr, 4);
    uint32_t d1 = lnaddr_read(descriptor_addr + 4, 4);
    
    // 正确解析段基址
    uint32_t base = (d0 >> 16) | ((d1 & 0xFF) << 16) | ((d1 & 0xFF000000));
    
    // 正确解析段限制
    uint32_t limit = (d0 & 0xFFFF) | ((d1 & 0x000F0000));
    if (d1 & 0x00800000) { // 检查G位
        limit = (limit << 12) | 0xFFF;
    }
    
    // 更新段寄存器信息
    cpu.sreg[1].base = base;
    cpu.sreg[1].limit = limit;
    
    // 更新EIP（指向目标地址）
    cpu.eip = offset;
    
    print_asm("ljmp $0x%x, $0x%x", selector, offset);
    return 7; // ljmp指令长度
}
#endif

#include "cpu/exec/template-end.h"
