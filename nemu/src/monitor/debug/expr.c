#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

#include<string.h>

int var_interface(char*token_str);

enum {
	NOTYPE = 256, EQ,NEQ,

	/* TODO: Add more token types */
	DEC_NUM,     //十进制数
	HEX_NUM,	//十六进制数
	REG,		//register
	VAR,		//变量
	MINUS, 		//一元负号
	DEREF,		//解引用符号*
	OR,AND		//LE,GE暂且不需要
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\(",'('},					//left
	{"\\)",')'},					//right

	{"\\+", '+'},					// plus
	{"-",'-'},						//sub
	{"\\*",'*'},					//multiply
	{"/",'/'},						//divide

	{"\\|\\|",OR},
	{"&&",AND},

	{"==", EQ},						//equal
	{"!=",NEQ},						//not equal
	// {"<",'<'},						//less than sign
	// {">",'>'},						//greater than sign
	//{"<=",LE},						less than or equal
	//{">=",GE},						greater than or equal
	{"!",'!'},

	{"[1-9][0-9]*", DEC_NUM},		//10进制数
	{"0[xX][0-9a-fA-F]+",HEX_NUM},	//16进制数
	{"\\$[a-zA-Z]+",REG},			//register
	{"[a-zA-Z_][0-9a-zA-Z_]*",VAR}		//符号变量
};


// 规则的数量
#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool is_operator(int args){
	switch(args){
		case '+':
		case '-':
		case '*':
		case '/':
		case EQ:
		case NEQ:
		case AND:
		case OR:
		case '!':
		case MINUS:
		case DEREF:return true;
		default:return false;
	}
}

// 词法分析
static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE:
						break;
					case DEC_NUM:{
						tokens[nr_token].type=DEC_NUM;
						if(substr_len>=32){
							assert("DEC_Number is too long.");
						}
						strncpy(tokens[nr_token].str,substr_start,substr_len);
						tokens[nr_token].str[substr_len]='\0';
						nr_token++;
						break;
					}
					case HEX_NUM:{
						tokens[nr_token].type=HEX_NUM;
						if(substr_len>=32){
							assert("HEX_Number is too long.");
						}
						strncpy(tokens[nr_token].str,substr_start+2,substr_len-2);
						tokens[nr_token].str[substr_len-2]='\0';
						nr_token++;
						break;
					}
					case REG:{
						tokens[nr_token].type=REG;
						if(substr_len>=32){
							assert("REG is too long.");
						}
						strncpy(tokens[nr_token].str,substr_start+1,substr_len-1);
						tokens[nr_token].str[substr_len-1]='\0';
						nr_token++;
						break;
					}
					case VAR:{
						tokens[nr_token].type=VAR;
						if(substr_len>=32){
							assert("VAR is too long.");
						}
						strncpy(tokens[nr_token].str,substr_start,substr_len);
						tokens[nr_token].str[substr_len]='\0';
						nr_token++;
						break;
					}
					case '+':{
						tokens[nr_token++].type=rules[i].token_type;
						break;
					}
					case '-':{
						if(nr_token==0||tokens[nr_token-1].type=='('||is_operator(tokens[nr_token-1].type)){
							tokens[nr_token++].type=MINUS;		//负号
							break;
						}
						else{
							tokens[nr_token++].type=rules[i].token_type;
							break;
						}
					}
					case '*':{
						if(nr_token==0||tokens[nr_token-1].type=='('||is_operator(tokens[nr_token-1].type)){
							tokens[nr_token++].type=DEREF;		//解引用符号
							break;
						}
						else{
							tokens[nr_token++].type=rules[i].token_type;
							break;
						}
					}
					case '/':
					case EQ:
					case NEQ:
					case '!':
					case AND:
					case OR:
					case '(':
					case ')':{
						tokens[nr_token++].type=rules[i].token_type;
						break;
					}
					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

static bool check_parentheses(int p,int q){
	int i,depth=2;
	if(tokens[p].type=='('&&tokens[q].type==')'){
		depth=1;
		for(i=p+1;i<q;i++){
			if(tokens[i].type=='('){
				depth++;
			}
			else if(tokens[i].type==')'){
				depth--;
				if(depth==0){
					return false;
				}
			}
		}
	}
	return depth==1;
}

static int find_dominant_operation(int p,int q){
	int i,depth=0,pos=-1,min_priority=4;
	for(i=p;i<=q;i++){
		// 可以把括号看作一种深度，检查到左括号时深度加1，右括号时深度减1，在深度为0时检测是否有op
		if(tokens[i].type=='('){
			depth++;
			continue;
		}
		if(tokens[i].type==')'){
			depth--;
			continue;
		}
		if(depth==0){
			int current_priority;
			switch(tokens[i].type){
				case OR:
				case AND:
				case EQ:
				case NEQ:{
					current_priority=0;
					break;
				}
				case '+':
				case '-': {
					current_priority=1;
					break;
				}
				case '*':
				case '/':{
					current_priority=2;
					break;
				}
				case MINUS:
				case DEREF:{
					current_priority=3;  //单目运算符
				}
				default:continue;
			}
			if(current_priority<=min_priority){
				min_priority=current_priority;
				pos=i;
			}
		}
	}
	return pos;
}

// evaluate:计算表达式的函数[递归求值]
static uint32_t eval(int p,int q){
	if(p>q){
		assert("p>q:You are false.");
	}
	else if(p==q){
		if(tokens[p].type==DEC_NUM){
			uint32_t ans;
			sscanf(tokens[p].str,"%d",&ans);
			return ans;
		}
		else if(tokens[p].type==HEX_NUM){
			uint32_t ans;
			sscanf(tokens[p].str,"%x",&ans);
			return ans;
		}
		else if(tokens[p].type==REG){
			uint32_t ans=-1;
			if(strcmp(tokens[p].str,"eax")==0){
				ans=cpu.eax;
			}
			else if(strcmp(tokens[p].str,"ecx")==0){
				ans=cpu.ecx;
			}
			else if(strcmp(tokens[p].str,"edx")==0){
				ans=cpu.edx;
			}
			else if(strcmp(tokens[p].str,"ebx")==0){
				ans=cpu.ebx;
			}
			else if(strcmp(tokens[p].str,"esp")==0){
				ans=cpu.esp;
			}
			else if(strcmp(tokens[p].str,"ebp")==0){
				ans=cpu.ebp;
			}
			else if(strcmp(tokens[p].str,"esi")==0){
				ans=cpu.esi;
			}
			else if(strcmp(tokens[p].str,"edi")==0){
				ans=cpu.edi;
			}
			else if(strcmp(tokens[p].str,"eip")==0){
				ans=cpu.eip;
			}
			else{
				assert("The reg doesn't exist in the register_list");
			}
			return ans;
		}
		else if(tokens[p].type==VAR){
			int ans=var_interface(tokens[p].str);
			if(ans==-1){
				assert("VAR is false!");
			}
			return ans;
		}
		else{
			printf("nihao,i'm %s ,type: %c\n",tokens[p].str,tokens[p].type);
			assert("p==q is false");
		}
	}
	else if(check_parentheses(p,q)==true){
		// Log("check_parentheses is false!!");//检查错误时使用的
		return eval(p+1,q-1);
	}
	else{
		int op=find_dominant_operation(p,q);
		if(p==op&&(tokens[op].type==MINUS||tokens[op].type==DEREF||tokens[op].type=='!')){
			switch(tokens[op].type){
				case MINUS:{
					return -eval(p+1,q);
				}
				case DEREF:{
					uint32_t addr=eval(p+1,q);
					uint32_t data=swaddr_read(addr,4);
					return data;
				}
				case '!':{
					return !eval(p+1,q);
				}
			}
		}
		uint32_t val1=eval(p,op-1);
		uint32_t val2=eval(op+1,q);
		switch(tokens[op].type){
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/':return val1/val2;
			case EQ:return val1==val2;
			case NEQ:return val1!=val2;
			case OR:return val1||val2;
			case AND:return val1&&val2;
			default :assert(0&&"Op is fasle!");
		}
	}
	return -1;
}


// 接口函数，可以在ui.c的cmd_p命令中调用，实现表达式求值
uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	uint32_t result = eval(0,nr_token-1);
	return result;
}

