#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "jle-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "jle-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "jle-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

// 所有helper_v函数的定义
make_helper_v(jle_si)