#ifndef __APP_PROC_H__
#define __APP_PROC_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>


typedef struct
{
    FILE*   fp_boot;
    FILE*   fp_app;
    FILE*   fp_asm;
    FILE*   fp_flashimage;
    FILE*   fp_json;
    uint8_t ver_major;
    uint8_t ver_minor;
    uint8_t swd_crp;
}app_proc_ctx_t;

typedef struct
{
    char     type[64];
    char     addr[64];
    uint32_t size_KB;
} element_t;


#endif /* __APP_PROC_H__*/


