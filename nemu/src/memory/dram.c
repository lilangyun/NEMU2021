#include "common.h"
#include "burst.h"
#include "misc.h"

/* Simulate the (main) behavor of DRAM.
 * Although this will lower the performace of NEMU, it makes
 * you clear about how DRAM perform read/write operations.
 * Note that cross addressing is not simulated.
 */

#define COL_WIDTH 10
#define ROW_WIDTH 10
#define BANK_WIDTH 3
#define RANK_WIDTH (27 - COL_WIDTH - ROW_WIDTH - BANK_WIDTH)

typedef union {
	struct {
		uint32_t col	: COL_WIDTH;	//2^10=1024说明有1024行
		uint32_t row	: ROW_WIDTH;	//同理，有1024列
		uint32_t bank	: BANK_WIDTH;	//8个bank
		uint32_t rank	: RANK_WIDTH;	//16个rank
	};//27位地址+剩下5位空
	uint32_t addr;
} dram_addr;


#define NR_COL (1 << COL_WIDTH)
#define NR_ROW (1 << ROW_WIDTH)
#define NR_BANK (1 << BANK_WIDTH)
#define NR_RANK (1 << RANK_WIDTH)

#define HW_MEM_SIZE (1 << (COL_WIDTH + ROW_WIDTH + BANK_WIDTH + RANK_WIDTH))	//内存大小：2^27B

uint8_t dram[NR_RANK][NR_BANK][NR_ROW][NR_COL];	//其中的每个元素表示某字节地址
uint8_t *hw_mem = (void *)dram;

typedef struct {
	uint8_t buf[NR_COL];
	int32_t row_idx;
	bool valid;
} RB;	//行缓冲区

RB rowbufs[NR_RANK][NR_BANK];

void init_ddr3() {
	int i, j;
	for(i = 0; i < NR_RANK; i ++) {
		for(j = 0; j < NR_BANK; j ++) {
			rowbufs[i][j].valid = false;
		}
	}
}//初始化行缓冲区,valid为false表示缓冲区均无效

static void ddr3_read(hwaddr_t addr, void *data) {
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);

	dram_addr temp;
	temp.addr = addr & ~BURST_MASK;	//BURST读取
	uint32_t rank = temp.rank;
	uint32_t bank = temp.bank;
	uint32_t row = temp.row;
	uint32_t col = temp.col;

	if(!(rowbufs[rank][bank].valid && rowbufs[rank][bank].row_idx == row) ) {
		/* read a row into row buffer */
		memcpy(rowbufs[rank][bank].buf, dram[rank][bank][row], NR_COL);
		rowbufs[rank][bank].row_idx = row;
		rowbufs[rank][bank].valid = true;	//表示该行缓冲区已占用
	}

	/* burst read */
	memcpy(data, rowbufs[rank][bank].buf + col, BURST_LEN);	//数据data类型为void*
	// burst_len表示读取8个字节即一个dram块有8字节内存
}

static void ddr3_write(hwaddr_t addr, void *data, uint8_t *mask) {
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);

	dram_addr temp;
	temp.addr = addr & ~BURST_MASK;
	uint32_t rank = temp.rank;
	uint32_t bank = temp.bank;
	uint32_t row = temp.row;
	uint32_t col = temp.col;

	if(!(rowbufs[rank][bank].valid && rowbufs[rank][bank].row_idx == row) ) {
		/* read a row into row buffer */
		memcpy(rowbufs[rank][bank].buf, dram[rank][bank][row], NR_COL);
		rowbufs[rank][bank].row_idx = row;
		rowbufs[rank][bank].valid = true;
	}

	/* burst write */
	memcpy_with_mask(rowbufs[rank][bank].buf + col, data, BURST_LEN, mask);

	/* write back to dram */
	memcpy(dram[rank][bank][row], rowbufs[rank][bank].buf, NR_COL);
}

// uint32_t dram_read(hwaddr_t addr, size_t len) {
// 	uint32_t offset = addr & BURST_MASK;	//取偏移量
// 	uint8_t temp[2 * BURST_LEN];	//burst_len起什么作用？
	
// 	ddr3_read(addr, temp);

// 	if(offset + len > BURST_LEN) {
// 		/* data cross the burst boundary */
// 		ddr3_read(addr + BURST_LEN, temp + BURST_LEN);
// 	}

// 	return (unalign_rw(temp + offset, 4))&(~0u >> ((4 - len) << 3));
// }
uint32_t dram_read(hwaddr_t addr, size_t len) {
  Assert(len == 1 || len == 2 || len == 4, "dram_read: invalid len %ld", len);
  uint32_t offset = addr & BURST_MASK;  // burst块内偏移（0~7）
  uint8_t temp[2 * BURST_LEN];          // 暂存2个burst块（16字节）
  memset(temp, 0, sizeof(temp));        // 初始化，避免脏数据干扰

  // 1. 读第一个burst块（按对齐地址读）
  hwaddr_t align_addr = addr & ~BURST_MASK;  // burst对齐地址（低3位置0）
  ddr3_read(align_addr, temp);

  // 2. 若跨burst块，读第二个burst块
  if (offset + len > BURST_LEN) {
    ddr3_read(align_addr + BURST_LEN, temp + BURST_LEN);
  }

  // 3. 关键修复：按实际len读取数据，并截取有效部分
  uint32_t full_data = unalign_rw(temp + offset, 4);  // 先读4字节
  // 按len截取有效数据（如len=1取低8位，len=2取低16位，len=4取完整32位）
  uint32_t mask = ~0u >> ((4 - len) << 3);
  return full_data & mask;
}

void dram_write(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t offset = addr & BURST_MASK;
	uint8_t temp[2 * BURST_LEN];
	uint8_t mask[2 * BURST_LEN];
	memset(mask, 0, 2 * BURST_LEN);

	*(uint32_t *)(temp + offset) = data;
	memset(mask + offset, 1, len);

	ddr3_write(addr, temp, mask);

	if(offset + len > BURST_LEN) {
		/* data cross the burst boundary */
		ddr3_write(addr + BURST_LEN, temp + BURST_LEN, mask + BURST_LEN);
	}
}

// 根据csdn上学长方法,制造外部可用函数
void ddr3_read_external(hwaddr_t addr, void *data){
	ddr3_read(addr,data);
}