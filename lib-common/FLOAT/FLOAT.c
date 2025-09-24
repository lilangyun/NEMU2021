#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	FLOAT ans=((long long)a*(long long)b)>>16;
	return ans;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	/* Dividing two 64-bit integers needs the support of another library
	 * `libgcc', other than newlib. It is a dirty work to port `libgcc'
	 * to NEMU. In fact, it is unnecessary to perform a "64/64" division
	 * here. A "64/32" division is enough.
	 *
	 * To perform a "64/32" division, you can use the x86 instruction
	 * `div' or `idiv' by inline assembly. We provide a template for you
	 * to prevent you from uncessary details.
	 *
	 *     asm volatile ("??? %2" : "=a"(???), "=d"(???) : "r"(???), "a"(???), "d"(???));
	 *
	 * If you want to use the template above, you should fill the "???"
	 * correctly. For more information, please read the i386 manual for
	 * division instructions, and search the Internet about "inline assembly".
	 * It is OK not to use the template above, but you should figure
	 * out another way to perform the division.
	 */
	FLOAT ans=(a/b)<<16;
	return ans;
}

FLOAT f2F(float a) {
	/* You should figure out how to convert `a' into FLOAT without
	 * introducing x87 floating point instructions. Else you can
	 * not run this code in NEMU before implementing x87 floating
	 * point instructions, which is contrary to our expectation.
	 *
	 * Hint: The bit representation of `a' is already on the
	 * stack. How do you retrieve it to another variable without
	 * performing arithmetic operations on it directly?
	 */
	/* 关键点在于：这里无法使用由浮点数到整型的转换（x87才提供），那么我们就需要采用指针的方法。
	 * 这里可以想到swaddr函数，但并不能使用。我们可以使用c语言标准库中的函数memcpy提取字节
	 *查阅资料才发现，原来float类型的小数点位是在23位处，分别占用位数为1，8，23
	 *此外，除了位数的问题，还应当考虑关于float类型，阶码与整数之间的关系，这些都属于上学期所学知识。
	 *int*arr;
	 *memcpy(arr,&a,4);
	 *int t=*arr;
	 *在看另一位学长的代码时发现了更加简便的方式
	 */
	int t=*(int*)&a;	//类型双关直接获取浮点数2进制位模式
	int sign=t>>31;	//符号位
	int exp=(t>>23)&0xff;	//阶码
	int frac=t&0x7fffff;	//尾数部分
	int ans=frac;			//返回结果
	// 首先考虑规格化数
	if(exp!=0){
		//隐式的1
		ans+=(1<<23);
		// 这里我理解2^E中E的值其实代表了1.frac需要左移的位数【而这里减去23显然目标是为了截断】
		// 因为接下来算出来的exp真正作用是为了右移【非常有趣的思路】
		exp=exp-(127+23)+16;
		if(exp>0){
			ans=ans<<exp;
		}
		else if(exp<0){
			ans=ans>>(-exp);
		}
		else{
			ans=ans;
		}
	}
	if(sign){
		return -ans;
	}
	return ans;
}

FLOAT Fabs(FLOAT a) {
	return a<0?-a:a;
}

/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

