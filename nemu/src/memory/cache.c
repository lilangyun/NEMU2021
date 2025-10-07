#include "memory/cache.h"
#include "burst.h"

void init_cache(){
    int i,j;
    for(i=0;i<CACHE_SET;i++){
        for(j=0;j<line_num;j++){
            cache_block[i][j].valid=false;
        }
    }
    srand(time(0)); //种子
}   //初始化cache

//cache读操作
void cache_read(hwaddr_t addr,void *data){
    CACHE_ADDR cache_addr;
    cache_addr.tag=(addr>>(set_num+offset_size))&((1<<tag_size)-1);
    cache_addr.set_index=(addr>>offset_size)&(CACHE_SET-1);
    uint32_t tag=cache_addr.tag;
    uint32_t set_index=cache_addr.set_index;
    // 清空块内偏移量找到块的起始位置
    uint32_t block_start=((addr>>offset_size)<<offset_size);
    int i;
    for(i=0;i<line_num;i++){
        // 如果匹配成功
        if(cache_block[set_index][i].valid&&cache_block[set_index][i].tag==tag){
            memcpy(data,cache_block[set_index][i].block,block_size);    
            //在此处块内偏移量并不重要，具体应该在hwaddr_read中实现偏移量
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
    if(!flag){
        int random_index=rand()%line_num;
        i=random_index;
    }
    // 很容易发现问题：读取长度不匹配！
    int j;
    for(j=0;j<block_size/BURST_LEN;j++){    //solution:这里我最喜欢（block_size/BURST_LEN）这一神来之笔
        ddr3_read_external(block_start+BURST_LEN*j,cache_block[set_index][i].block+BURST_LEN*j); 
        //一步到位
    }
    // 当从dram中读取之后，我们还要写入data中，否则根本毫无作用
    memcpy(data,cache_block[set_index][i].block,block_size);
    return ;
}

// cache写操作
void cache_write(hwaddr_t addr,uint32_t data,size_t len){
    CACHE_ADDR cache_addr;
    cache_addr.tag=(addr>>(set_num+offset_size))&((1<<tag_size)-1);
    cache_addr.set_index=(addr>>offset_size)&(CACHE_SET-1);
    cache_addr.offset=addr&(block_size-1);
    uint32_t tag=cache_addr.tag;
    uint32_t set_index=cache_addr.set_index;
    uint32_t offset=cache_addr.offset;  //写操作需要补充这段代码
    int i;
    bool hit=false;
    for(i=0;i<line_num;i++){
        // 如果匹配成功
        if(cache_block[set_index][i].valid&&cache_block[set_index][i].tag==tag){
            hit=true;
            break;
        }
    }
    // 如果命中
    if(hit){
        // 判断是否会出现跨越块的情况
        if(offset+len>block_size){
            int t=block_size-offset;
            memcpy(cache_block[set_index][i].block+offset,&data,t);
            cache_write(addr+t,data>>((4-(len-t))<<3),len-t);
            return ;
        }
        else{
            memcpy(cache_block[set_index][i].block+offset,&data,len);
        }
    }
    // 未命中：采用非写分配,直接写入内存
    dram_write(addr,len,data);
}