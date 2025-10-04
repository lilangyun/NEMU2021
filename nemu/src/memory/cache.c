#include "common.h"

#define valid_bit 1 //有效位
#define offset_size 6   //块内偏移量
#define set_num 7   //64*1024B/64*8B==2^(10-3)
#define tag_size (32-offset_size-set_num)   //19位
#define line_num 8  //8组相联：每组有8块cache_block
#define block_size 64

#define NR_SET (1<<set_num)
#define NR_LINE (1<<line_num)

typedef struct{
    uint32_t tag:tag_size;
    uint32_t offset:offset_size;
    uint32_t set_index:set_num;
}cache_addr;    //用于找cache块的地址

typedef struct{
    bool valid:valid_bit;
    uint32_t tag:tag_size;
    uint8_t block[block_size];
}CB;   //cache块

CB cache_block[NR_SET][NR_LINE];
