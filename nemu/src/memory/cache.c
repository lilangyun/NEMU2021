#include "common.h"

#define valid_bit 1 //有效位
#define offset_size 6   //块内偏移量
#define set_num 7   //64*1024B/64*8B==2^(10-3)
#define tag_size (32-offset_size-set_num)   //19位


#define line_num 8  //8组相联：每组有8块cache_block
#define block_size 64
#define NR_SET (1<<set_num)

void hwaddr_write(hwaddr_t, size_t, uint32_t);  //为了write-through

typedef struct{
    uint32_t tag:tag_size;
    uint32_t set_index:set_num;
    uint32_t offset:offset_size;
}CA;    //用于找cache块的地址:cache_addr

typedef struct{
    bool valid:valid_bit;
    uint32_t tag:tag_size;
    uint8_t block[block_size];
}CB;   //cache块:cache_block

CB cache_block[NR_SET][line_num];

void init_cache(){
    int i,j;
    for(i=0;i<NR_SET;i++){
        for(j=0;j<line_num;j++){
            cache_block[i][j].valid=false;
        }
    }
}   //初始化cache

static void cache_read(hwaddr_t addr,void *data){
    CA cache_addr;
    cache_addr.tag=(addr>>(set_num+offset_size))&((1<<tag_size)-1);
    cache_addr.set_index=(addr>>offset_size)&((1<<set_num)-1);
    cache_addr.offset=addr&((1<<offset_size)-1);
    uint32_t tag=cache_addr.tag;
    uint32_t set_index=cache_addr.set_index;
    uint32_t offset=cache_addr.offset;

    int i;
    for(i=0;i<line_num;i++){
        // 如果该块未满足要求
        if(!(cache_block[set_index][i].valid&&cache_block[set_index][i].tag==tag)){
            memcpy(cache_block[set_index][i].block+offset,,1);  //1字节代表数据大小--之后可于头文件中宏定义
            cache_block[set_index][i].valid=true;
            cache_block[set_index][i].tag=tag;
        }
        // 满足要求
        else{
            memcpy(data,cache_block[set_index][i].block+offset,1);  //写入参数data中
            return ;
        }
    }
}