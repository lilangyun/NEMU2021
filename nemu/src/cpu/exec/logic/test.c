#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "test-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "test-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "test-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

// 所有helper函数的定义
make_helper_v(test_rm_r)
