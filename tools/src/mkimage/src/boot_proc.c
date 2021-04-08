#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "boot_type.h"
#include "crc32.h"
#include "flash_map_ln88xx.h"

static boot_header_t s_boot_header = { 0 };
////----------------------(vender define)-------------------------////
#define BOOT_SIZE_LIMITE     (1024 * 4 * 9)//36KB

int boot_proc(FILE* fp_boot, uint8_t swd_crp)
{
	boot_header_t* boot_header = &s_boot_header;
	uint32_t boot_fsize = 0, rd_len = 0;
	uint32_t crc_result = 0;
	char* buffer = NULL;

	//read boot_header
	fread(boot_header, sizeof(boot_header_t), 1, fp_boot);

	//get file size, write bootram_bin_length
	fseek(fp_boot, 0, SEEK_END);
	boot_fsize = ftell(fp_boot);
	if (boot_fsize > BOOT_SIZE_LIMITE) {
		printf("boot.bin is too large, file_size = %d, file_limit = %d\n", boot_fsize, BOOT_SIZE_LIMITE);
		return -1;
	}
	boot_header->bootram_bin_length = boot_fsize;
	printf("boot.bin total_len = %d\n", boot_fsize);

	//write crp_flag
	if (swd_crp == 0) {
		boot_header->crp_flag = 0;
	} else {
		boot_header->crp_flag = CRP_VALID_FLAG;
	}

	//read remaining boot files, cal bootram_crc32
	if (NULL == (buffer = (char*)malloc(boot_fsize - sizeof(boot_header_t)))){
		printf("Malloc memory for boot_ram.bin fail.\n");
		return -1;
	}
	rewind(fp_boot);
	fseek(fp_boot, boot_header->bootram_crc_offset, SEEK_SET);
	rd_len = fread(buffer, 1, (boot_fsize - boot_header->bootram_crc_offset), fp_boot);
	if (rd_len != (boot_fsize - boot_header->bootram_crc_offset)){
		printf("read boot.bin error\r\n");
		return -1;
	}
	crc_result = GetCrc32(buffer, rd_len);
	boot_header->bootram_crc_value = crc_result;
	printf("boot_ram.bin CRC32 = 0x%08X\r\n", crc_result);

	//cal boot header crc32
	crc_result = GetCrc32((uint8_t*)boot_header, (sizeof(boot_header_t) - sizeof(uint32_t)));
	boot_header->boot_hearder_crc = crc_result;
	printf("boot_header CRC32 = 0x%08X\n", crc_result);

	//update boot header to file
	rewind(fp_boot);
	fwrite(boot_header, 1, sizeof(boot_header_t), fp_boot);

	free(buffer);
	return 0;
}



