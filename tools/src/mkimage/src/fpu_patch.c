#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fpu_patch.h"

static bool str2hex(char* s, uint8_t s_len_max, uint32_t* p_hex)
{
    uint32_t n = 0;
    uint8_t  i = 0;

    while (s[i] != '\0' && s[i] != '\n')
    {
        if (s[i] >= '0' && s[i] <= '9') {
            n = n * 16 + (s[i] - '0');
        }
        else if (s[i] >= 'a' && s[i] <= 'f') {
            n = n * 16 + (s[i] - 'a') + 10;
        }
        else if (s[i] >= 'A' && s[i] <= 'F') {
            n = n * 16 + (s[i] - 'A') + 10;
        }
        else {
            return false;
        }

        if (++i > s_len_max) {
            return false;
        }
    }

    *p_hex = n;
    return true;
}

static bool find_offset(char* str, char* pos_end, uint32_t* offset)
{
    uint32_t code_offset = 0;
    char code_offset_str[9];
    char* pos_start = NULL;
    uint8_t str_len = 0;

    pos_start = strstr(str, "        0x");
    pos_start += strlen("        0x");
    str_len = (uint8_t)(pos_end - pos_start);

    if (str_len != 8) {
        goto error_end;
    }
    memset(code_offset_str, '\0', sizeof(code_offset_str));
    memcpy(code_offset_str, pos_start, 8);

    if (!str2hex(code_offset_str, 8, &code_offset)) {
        goto error_end;
    }

    *offset = code_offset;
    return true;

error_end:
    return false;
}

bool code_replace(FILE* fp_bin, inst_type_enum type, char* pos_inst, uint32_t offset)
{
    uint16_t mach_code_1 = 0, mach_code_2 = 0;
    uint8_t rd_buf[4], wr_buf[4];
    size_t  ret_len = 0;

    fseek(fp_bin, offset - CACHE_FLASH_BASE, SEEK_SET);
    ret_len = fread(&rd_buf, sizeof(uint8_t), 4, fp_bin);
    if (ret_len != 4) {
        printf("read flashimage.bin error!\n");
        goto run_error;
    }

    mach_code_1 = rd_buf[0];
    mach_code_1 += (uint16_t)(rd_buf[1] << 8);

    mach_code_2 = rd_buf[2];
    mach_code_2 += (uint16_t)(rd_buf[3] << 8);

    switch (type)
    {
        case INST_VMLA:
            mach_code_1 |= (uint16_t)(1 << 5);
            mach_code_1 |= (uint16_t)(1 << 7);

            //code check op
            if ((mach_code_2 & 0x0040) != 0) {
                goto run_error;
            }
            break;
        case INST_VNMLA:
            mach_code_1 |= (uint16_t)(1 << 7);

            //code check op
            if ((mach_code_2 & 0x0040) == 0) {
                goto run_error;
            }
            break;
        case INST_VMLS:
            mach_code_1 |= (uint16_t)(1 << 5);
            mach_code_1 |= (uint16_t)(1 << 7);

            //code check op
            if ((mach_code_2 & 0x0040) == 0) {
                goto run_error;
            }
            break;
        case INST_VNMLS:
            mach_code_1 |= (uint16_t)(1 << 7);

            //code check op
            if ((mach_code_2 & 0x0040) != 0) {
                goto run_error;
            }
            break;
        default:
            goto run_error;
    }

    //code check sz
    if ((mach_code_2 & 0x0100) != 0) {
        goto run_error;
    }

    wr_buf[0] = (uint8_t)(mach_code_1 & 0x00ff);
    wr_buf[1] = (uint8_t)((mach_code_1 & 0xff00) >> 8);
    wr_buf[2] = (uint8_t)(mach_code_2 & 0x00ff);
    wr_buf[3] = (uint8_t)((mach_code_2 & 0xff00) >> 8);

    fseek(fp_bin, offset - CACHE_FLASH_BASE, SEEK_SET);
    ret_len = fwrite((uint8_t*)&wr_buf, sizeof(uint8_t), 4, fp_bin);
    fflush(fp_bin);
    if (ret_len != 4) {
        printf("write flashimage.bin error!\n");
        goto run_error;
    }

    return true;
run_error:
    return false;
}

int fpu_patch(FILE* fp_asm, FILE* fp_flashimage)
{
    //errno_t err;

    int i = 0;

    uint32_t mach_code_offset = 0;
    char* ret_rd = NULL;
    size_t  ret_len = 0;
    char* pos_end = NULL;
    char* pos_inst = NULL;
    uint32_t patch_count = 0;

    char* str_line = malloc(STR_LINE_LEN_MAX);
    if (str_line == NULL)
    {
        printf("脚本运行过程中内存申请失败！\n");
        goto run_error;
    }

    fseek(fp_asm, 0, SEEK_SET);

    do
    {
        memset(str_line, 0, STR_LINE_LEN_MAX);
        ret_rd = fgets(str_line, STR_LINE_LEN_MAX, fp_asm);

        if (strstr(str_line, "        0x") && (pos_end = strstr(str_line, ":    ee")) && (pos_inst = strstr(str_line, "    VMLA.F32 ")))
        {
//            printf("%s", str_line);

            if (!find_offset(str_line, pos_end, &mach_code_offset)) {
                printf("find offset error！\n");
                goto run_error;
            }
            if (!code_replace(fp_flashimage, INST_VMLA, pos_inst, mach_code_offset)) {
                printf("replace mach code error!\n");
                goto run_error;
            }
            patch_count++;
        }
        else if (strstr(str_line, "        0x") && (pos_end = strstr(str_line, ":    ee")) && (pos_inst = strstr(str_line, "    VNMLA.F32 ")))
        {
//            printf("%s", str_line);

            if (!find_offset(str_line, pos_end, &mach_code_offset)) {
                printf("find offset error!\n");
                goto run_error;
            }
            if (!code_replace(fp_flashimage, INST_VNMLA, pos_inst, mach_code_offset)) {
                printf("replace mach code error!\n");
                goto run_error;
            }
            patch_count++;
        }
        else if (strstr(str_line, "        0x") && (pos_end = strstr(str_line, ":    ee")) && (pos_inst = strstr(str_line, "    VMLS.F32 ")))
        {
//            printf("%s", str_line);

            if (!find_offset(str_line, pos_end, &mach_code_offset)) {
                printf("find offset error!\n");
                goto run_error;
            }
            if (!code_replace(fp_flashimage, INST_VMLS, pos_inst, mach_code_offset)) {
                printf("replace mach code error!\n");
                goto run_error;
            }
            patch_count++;
        }
        else if (strstr(str_line, "        0x") && (pos_end = strstr(str_line, ":    ee")) && (pos_inst = strstr(str_line, "    VNMLS.F32 ")))
        {
//            printf("%s", str_line);

            if (!find_offset(str_line, pos_end, &mach_code_offset)) {
                printf("find offset error!\n");
                goto run_error;
            }
            if (!code_replace(fp_flashimage, INST_VNMLS, pos_inst, mach_code_offset)) {
                printf("replace mach code error!\n");
                goto run_error;
            }
            patch_count++;
        }

    } while (ret_rd != NULL);

    printf("fpu patch count: %d\n", patch_count);

    free(str_line);
    printf("fpu patched successfully!\n");
    return 0;

run_error:
    free(str_line);
    printf("fpu patched failed!\n");
    return -1;
}

