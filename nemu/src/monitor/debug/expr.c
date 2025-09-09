#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

#include<string.h>

enum {
	NOTYPE = 256, EQ,

	/* TODO: Add more token types */
	TK_NUM,     //十进制数
	MINUS 		//一元负号
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
	{"==", EQ},						// equal
	{"[1-9][0-9]*", TK_NUM},		//10进制数
};

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
	if(args=='+'||args=='-'||args=='*'||args=='/'){
		return true;
	}
	return false;
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
					case TK_NUM:{
						tokens[nr_token].type=TK_NUM;
						if(substr_len>=32){
							assert(0&&"Number is too long.");
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
							tokens[nr_token++].type=MINUS;
							break;
						}
						else{
							tokens[nr_token++].type=rules[i].token_type;
							break;
						}
					}
					case '*':{
						tokens[nr_token++].type=rules[i].token_type;
						break;
					}
					case '/':{
						tokens[nr_token++].type=rules[i].token_type;
						break;
					}
					case '(':{
						tokens[nr_token++].type=rules[i].token_type;
						break;
					}
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
	int i,depth=1;
	if(tokens[p].type=='('&&tokens[q].type==')'){
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
	int i,depth=0,pos=-1,min_priority=3;
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

// evaluate:计算表达式的函数
static int eval(int p,int q){
	if(p>q){
		assert(0);
	}
	else if(p==q){
		if(tokens[p].type==TK_NUM){
			int ans;
			sscanf(tokens[p].str,"%d",&ans);
			return ans;
		}
		else{
			assert(0);
		}
	}
	else if(check_parentheses(p,q)==true){
		return eval(p+1,q-1);
	}
	else if(tokens[p].type==MINUS){
		return -eval(p+1,q);
	}
	else{
		int op=find_dominant_operation(p,q);
		int val1=eval(p,op-1);
		int val2=eval(op+1,q);
		switch(tokens[op].type){
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/':return val1/val2;
			default :assert(0&&"Op is fasle!");
		}
	}
}


// 接口函数，可以在ui.c的cmd_p命令中调用，实现表达式求值
uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	int result = eval(0, nr_token - 1);
	return result;
	panic("please implement me");
	return 0;
}

