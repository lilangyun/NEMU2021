#ifndef __OPERAND_H__
#define __OPERAND_H__

enum { OP_TYPE_REG, OP_TYPE_MEM, OP_TYPE_IMM, OP_TYPE_CR, OP_TYPE_SREG };  //添加了两种操作数类型

#define OP_STR_SIZE 40

typedef struct {
	uint32_t type;
	size_t size;
	union {
		uint32_t reg;
		// swaddr_t addr;
		struct{
			swaddr_t addr;
			uint8_t sreg;  //也就是说线性地址:=(sreg:addr);sreg位于高地址处
		};
		uint32_t imm;
		int32_t simm;
	};
	uint32_t val;
	char str[OP_STR_SIZE];
} Operand;

typedef struct {
	uint32_t opcode;
	bool is_operand_size_16; //原来在这里
	Operand src, dest, src2;
} Operands;//每个指令所对应的操作数的结构体定义

#endif
