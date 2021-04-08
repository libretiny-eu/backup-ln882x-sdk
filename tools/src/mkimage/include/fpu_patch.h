#ifndef __FPU_PATCH_H__
#define __FPU_PATCH_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#define STR_LINE_LEN_MAX  2000+2
#define CACHE_FLASH_BASE  0x10000000

typedef enum
{
    INST_VMLA = 0,
    INST_VNMLA = 1,
    INST_VMLS = 2,
    INST_VNMLS = 3,
}inst_type_enum;

int fpu_patch(FILE* fp_asm, FILE* fp_flashimage);


#endif /* __FPU_PATCH_H__*/






