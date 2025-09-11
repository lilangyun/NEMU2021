#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	char expr[32];	//表达式
	uint32_t v1;
	bool enable;

} WP;
WP* new_wp();
void free_wp(WP* wp);
bool check_all_wp_changed();
bool check_wp_changed(WP* wp);
void set_wp_expr(char *args);
void info_wp();
// void function1(char *);		//这里可能要修改，因为我做的接口其实不很好，是在wp文件外去对“池”进行访问的
#endif
