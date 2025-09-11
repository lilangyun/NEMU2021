#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32	//watchpoint池的总大小：32，还可以自行调整

static WP wp_pool[NR_WP];
static WP *head, *free_;	//两个链表用来连接：使用中的监视点结构；空闲的监视点结构
// static实现一种封装，保证安全性

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){	//return a free watchpoint from the free_	
	if(free_==NULL){
		printf("No free watchpoint!!");
		return NULL;
		assert(0&&"free_ is spare,it's busy!");
	}
	WP* wp=free_;
	free_=wp->next;
	wp->next=head;	//还要将wp放在已使用监视点head前面作为新的head
	head=wp;
	return wp;
}

void free_wp(WP* wp){	//the args (WP*wp) will return to free_
	// 要注重与已使用监视点的结合
	if(head==wp){
		head=wp->next;
	}
	else{
		WP*t=head;
		while(t->next!=wp){
			t=t->next;
		}
		t->next=wp->next;
	}
	wp->next=free_;
	free_=wp;
}

/* 实现监视表达式是否发生变化的功能，需要调用expr.c中的求值函数，同时也是一个接口函数，供ui.c调用
// 这时我意识到我应该要去给Wp添加：表达式，首次表达式求出来的值，执行cpu_exec后的值
// void function1(char *args){
// 	WP*wp=new_wp();
// 	bool success;
// 	wp->expr=args;
// 	wp->v1=expr(args, &success);
// }
// 这个函数要说再见了，直接写两个新函数*/
void set_wp_expr(char *args){
	WP*wp=new_wp();
	bool success;
	wp->expr=args;
	wp->v1=expr(args, &success);
	if(!success){
		printf("Wrong expression!\n");
		free_wp(wp);
	}
	else{
		printf("Set watchpoint %d: %s = %d\n",wp->NO,wp->expr,wp->v1);
	}
}