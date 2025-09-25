#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "js-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "js-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "js-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

// 所有helper_v函数的定义
make_helper_v(js_si)