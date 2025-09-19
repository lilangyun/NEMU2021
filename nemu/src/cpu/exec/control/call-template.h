#include "cpu/exec/template-start.h"

#define instr call

// 通用执行过程
static void do_execute(int len) {
    // 返回地址入栈
    cpu.esp-=DATA_BYTE;
    swaddr_write(cpu.esp,4,cpu.eip+1+len);//在这里，我犯了误区，事实上，我应该保存下一个指令的地址
    // 调用函数
    cpu.eip += op_src->val;//直接复制jmp中指令即可，但是竟然有差异
    // 好像到这里就结束了
    print_asm("call %x", cpu.eip+1+len);//像jmp一样打印一下吧，表示调用函数的位置
}
// 译码函数
//在此处神奇地发现，我将call-template.h函数更改后，suffix已经传入
make_helper(concat(call_si_, SUFFIX)) {
	int len=concat(decode_si_, SUFFIX)(eip+1);//提取操作数
	do_execute(len);
	return len+1;
}
#include "cpu/exec/template-end.h"