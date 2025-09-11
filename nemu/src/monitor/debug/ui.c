#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);//if -1 is unsined int , it's so big!
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
	int n=1;
	if(args!=NULL){
		sscanf(args,"%d",&n);
	}
	cpu_exec(n);
	return 0;
}

static int cmd_info(char *args){
	if(args==NULL){
		printf("Usage: info r\n");
		return 0;
	}
	else if(strcmp(args,"r")==0){
		printf("eax : 0x%08x\n",cpu.eax);
		printf("ecx : 0x%08x\n",cpu.ecx);
		printf("edx : 0x%08x\n",cpu.edx);
		printf("ebx : 0x%08x\n",cpu.ebx);
		printf("esp : 0x%08x\n",cpu.esp);
		printf("ebp : 0x%08x\n",cpu.ebp);
		printf("esi : 0x%08x\n",cpu.esi);
		printf("edi : 0x%08x\n",cpu.edi);
		printf("eip : 0x%08x\n",cpu.eip);
	}
	else if(strcmp(args,"w")==0){
		WP*wp=new_wp();
		while(wp->next!=NULL){
			wp=wp->next;
			printf("Hint watchpoint %d at address 0x%x\n",wp->NO,cpu.eip);
		}
	}
	return 0;
}

static int cmd_x(char *args){
	uint32_t addr;
	int n,i;
	char *args1=strtok(args," ");
	char *args2=strtok(NULL," ");
	sscanf(args1,"%d",&n);
	sscanf(args2,"%x",&addr);
	for(i=0;i<n;i++){
		uint32_t data=swaddr_read(addr+4*i, 4);
		printf("0x%08x : 0x%08x\n",addr+4*i,data);
	}
	return 0;
}

static int cmd_p(char *args){
	bool success;
	uint32_t t=expr(args,&success);
	printf("%s = %d\n",args,t);
	return 0;
}

static int cmd_w(char *args){
	// function1(args);
	return 0;
}

static int cmd_d(char *args){
	int n;
	bool flag=false;
	sscanf(args,"%d",&n);
	WP*wp=new_wp();
	while(wp->next!=NULL){
		wp=wp->next;
		if(wp->NO==n){
			free_wp(wp);
			flag=true;
			break;
		}
	}
	if(!flag){
		assert(0&&"watchpoint没有这个序号!!");
	}
	return 0;
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);//function pointer : arg is char* , returned value is int.
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands */
	{ "si","Input the n",cmd_si},
	{"info","Print the regs' states",cmd_info},
	{"x","Scan memory",cmd_x},
	{"p","Figure out the value of expression",cmd_p},
	{"w","Set up the watchpoint",cmd_w},
	{"d","Delete the watchpoint",cmd_d}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;	//这步有点看不太懂
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {	//遍历所有命令，寻找匹配项
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }	//目前就只有q指令返回-1满足要求
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
