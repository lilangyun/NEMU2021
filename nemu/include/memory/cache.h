#ifndef __cache_H__
#define __cache_H__

#include "common.h"
// 用于生成随机数
#include <stdlib.h>
#include <time.h>

#define valid_bit 1 //有效位
#define offset_size 6   //块内偏移索引
#define set_num 7   //64*1024B/64*8B==2^(10-3)组索引
#define tag_size (32-offset_size-set_num)   //19位


#define line_num 8  //8组相联：每组有8块cache_block
#define block_size 64
#define CACHE_SET (1<<set_num)

void ddr3_read_external(hwaddr_t,void*);
void dram_write(hwaddr_t, size_t, uint32_t);

typedef struct{
    uint32_t tag:tag_size;
    uint32_t set_index:set_num;
    uint32_t offset:offset_size;
}CACHE_ADDR;    //用于找cache块的地址:cache_addr

typedef struct{
    bool valid:valid_bit;   //如果valid==true，那么说明为有效数据；反之说明空数据
    uint32_t tag:tag_size;
    uint8_t block[block_size];
}CACHE_BLOCK;   //cache块:cache_block
CACHE_BLOCK cache_block[CACHE_SET][line_num];

void init_cache();
//cache读操作
void cache_read(hwaddr_t,void*);
void cache_write(hwaddr_t,uint32_t,size_t);

#endif