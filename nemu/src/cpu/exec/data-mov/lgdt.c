#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "lgdt-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "lgdt-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

// 所有helper函数的定义
make_helper_v(lgdt)