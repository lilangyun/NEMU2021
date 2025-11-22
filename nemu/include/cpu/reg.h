#ifndef __REG_H__
#define __REG_H__

#include "common.h"
#include "../../../lib-common/x86-inc/cpu.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI};
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };
enum { S_CS, S_SS, S_DS, S_ES};  //段寄存器编码

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
	union{
     	union {
			uint32_t _32;
			uint16_t _16;
			uint8_t _8[2];
     	} gpr[8];

     /* Do NOT change the order of the GPRs' definitions. */

    	struct{
			uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
	};
     swaddr_t eip;
    //  这里涉及到c位域的知识，与内存位置和地址有关
     union {
		// 有趣的是：这个union（也就是eflags这个寄存器）正好占据一个字节
		struct {
			uint32_t CF		:1;
			uint32_t pad0	:1;
			uint32_t PF		:1;
			uint32_t pad1	:1;
			uint32_t AF		:1;
			uint32_t pad2	:1;
			uint32_t ZF		:1;
			uint32_t SF		:1;
			uint32_t TF		:1;
			uint32_t IF		:1;
			uint32_t DF		:1;
			uint32_t OF		:1;
			uint32_t IOPL	:2;
			uint32_t NT		:1;
			uint32_t pad3	:1;
			uint16_t pad4;
		};
		uint32_t val;
	} eflags;  //标志位


	// We need to add GDTR,CR0,and CS,SS,DS,ES(16bits)
	struct{
		uint16_t lim;  //GDT长度
		uint32_t base;  //GDT基址，即段描述符表首地址
	}GDTR;  //段描述符表基址寄存器
	
	//控制寄存器
	CR0 cr0;
	CR3 cr3;

	struct{
		union{
			struct{
				uint16_t RPL:3;
				uint16_t TI:1;
				uint16_t INDEX:12;
			};
			uint16_t val;
		};
		
		uint16_t attribute;
		uint32_t limit;
		uint32_t base; 
	}sreg[4];

} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

static inline int check_sreg_index(int index) {
	assert(index >= 0 && index < 4);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

#define sreg_w(index) (cpu.sr[check_reg_index(index)].s_16)

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

#endif
