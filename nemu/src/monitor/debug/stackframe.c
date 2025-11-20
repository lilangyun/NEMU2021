#include "nemu.h"
#include "monitor/expr.h"

char* GetFunctionAddr(swaddr_t addr);

// 栈帧结构体
typedef struct {
    swaddr_t prev_ebp;
    swaddr_t ret_addr;
    uint32_t args[4];
}PartofStackFrame;


// 栈回溯函数
void backstack(){
    int cnt=0;    //表示栈帧序号：便于输出gdb格式
    // 最初我想法是去在指令中构造链表，但看了博客之后才发现栈帧链竟然是隐式形成的
    swaddr_t addr=cpu.ebp;
    swaddr_t eip=cpu.eip;
    // 从当前ebp开始
    while(addr){
        // 添加链表元素
        PartofStackFrame frame;  //当前栈帧节点
        int i;
        frame.prev_ebp=swaddr_read(addr+0,4,1);
        frame.ret_addr=swaddr_read(addr+4,4,1);  //ss表示1
        for(i=0;i<4;i++){
            frame.args[i]=swaddr_read(addr+8+4*i,4,1);
        }
        // 栈帧结束
        // 根据返回地址,获取函数名
        char*func_name=GetFunctionAddr(eip);
        printf("#%d 0x%08x in %s (0x%08x 0x%08x 0x%08x 0x%08x)\n",cnt,eip,func_name,frame.args[0],frame.args[1],frame.args[2],frame.args[3]);
        eip=frame.ret_addr;
        addr=frame.prev_ebp;  //这是栈帧链中ebp的位置
        cnt++;
    }
}