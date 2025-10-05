#include "common.h"
#include "burst.h"
// 用于生成随机数
#include <stdlib.h>
#include <time.h>

#define valid_bit 1 //有效位
#define offset_size 6   //块内偏移索引
#define set_num 7   //64*1024B/64*8B==2^(10-3)组索引
#define tag_size (32-offset_size-set_num)   //19位


#define line_num 8  //8组相联：每组有8块cache_block
#define block_size 64
#define NR_SET (1<<set_num)

void ddr3_read_external(hwaddr_t addr, void *data);

typedef struct{
    uint32_t tag:tag_size;
    uint32_t set_index:set_num;
    uint32_t offset:offset_size;
}CA;    //用于找cache块的地址:cache_addr

typedef struct{
    bool valid:valid_bit;   //如果valid==true，那么说明为有效数据；反之说明空数据
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

//cache读操作
void cache_read(hwaddr_t addr,void *data,size_t len){
    CA cache_addr;
    cache_addr.tag=(addr>>(set_num+offset_size))&((1<<tag_size)-1);
    cache_addr.set_index=(addr>>offset_size)&(NR_SET-1);
    cache_addr.offset=addr&(block_size-1);
    uint32_t tag=cache_addr.tag;
    uint32_t set_index=cache_addr.set_index;
    uint32_t offset=cache_addr.offset;

    // 清空块内偏移量
    uint32_t block_start=((addr>>offset_size)<<offset_size);

    srand(time(0)); //种子

    int i;
    for(i=0;i<line_num;i++){
        // 如果匹配成功
        if(cache_block[set_index][i].valid&&cache_block[set_index][i].tag==tag){
            memcpy(data,cache_block[set_index][i].block+offset,len);
            return ;
        }
    }
    // 若没有匹配成功：采用随机替换策略
    bool flag=false;
    for(i=0;i<line_num;i++){
        // 如果有空缓冲块
        if(cache_block[set_index][i].valid==false){
            flag=true;
            break;
        }
    }

    void*temp;
    if(!flag){
        int random_index=rand()%line_num;
        i=random_index;
    }
    // 很容易发现问题：读取长度不匹配！
    int j;
    for(j=0;j<block_size/BURST_LEN;i++){    //solution:这里我最喜欢（block_size/BURST_LEN）这一神来之笔
        ddr3_read_external(block_start+BURST_LEN*j,cache_block[set_index][i].block+BURST_LEN*j); 
        //一步到位
    }
    return ;
}

