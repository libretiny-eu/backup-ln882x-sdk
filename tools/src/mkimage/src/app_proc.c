#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "boot_type.h"
#include "crc32.h"
#include "flash_map_ln88xx.h"
#include "cJSON.h"

#include "app_proc.h"
#include "fpu_patch.h"

static FILE* fp_boot = NULL;
static FILE* fp_app = NULL;
static FILE* fp_asm = NULL;
static FILE* fp_flashimage = NULL;
static FILE* fp_json = NULL;
static partition_info_t  kv_part, simu_eeprom_part, app_part, ota_part, nvds_part, user_part;
static uint32_t boot_partition_size = 0;
static uint32_t partition_tab_base = 0;

static cJSON* GetJsonObject(FILE* fp)
{
	long len;
	char* content;
	static cJSON* root = NULL;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	if (0 == len) {
		return NULL;
	}

	content = (char*)malloc(sizeof(char) * len);
	fseek(fp, 0, SEEK_SET);

	if (len != fread(content, 1, len, fp)) {
		return NULL;
	}

	root = cJSON_Parse(content);
	if (!root) {
		return NULL;
	}

	free(content);
	return root;
}

static bool build_partition_table(FILE* fp, uint8_t* buf_4k)
{
	cJSON* root, * temp1, * temp2;
	if (NULL == (root = GetJsonObject(fp))) {
		printf("json file parse filed!\n");
		return false;
	}

	cJSON* user_define = cJSON_GetObjectItem(root, "user_define");
	uint8_t user_size = cJSON_GetArraySize(user_define);

	cJSON* vender_define = cJSON_GetObjectItem(root, "vendor_define");
	uint8_t vender_size = cJSON_GetArraySize(vender_define);

	element_t  e_boot, e_part_tab, e_kv, e_simu_eeprom, e_app, e_ota, e_nvds, e_user;
	char* ptr;

	memset(&e_boot, 0, sizeof(element_t));
	memset(&e_part_tab, 0, sizeof(element_t));

	memset(&e_kv, 0, sizeof(element_t));
	memset(&e_simu_eeprom, 0, sizeof(element_t));
	memset(&e_app, 0, sizeof(element_t));
	memset(&e_ota, 0, sizeof(element_t));
	memset(&e_nvds, 0, sizeof(element_t));
	memset(&e_user, 0, sizeof(element_t));

	for (uint8_t i = 0; i < vender_size; i++) {
		temp1 = cJSON_GetArrayItem(vender_define, i);
		temp2 = cJSON_GetObjectItem(temp1, K_PARTITION_TYPE);

		if (!strcmp(V_BOOT, temp2->valuestring)) {
			temp2 = cJSON_GetObjectItem(temp1, K_SIZE_KB);
			e_boot.size_KB = temp2->valueint;

			boot_partition_size = e_boot.size_KB * 1024;
		}
		else if (!strcmp(V_PART_TAB, temp2->valuestring)) {
			temp2 = cJSON_GetObjectItem(temp1, K_START_ADDR);
			memcpy(&e_part_tab.addr, temp2->valuestring, strlen(temp2->valuestring));

			partition_tab_base = strtol(e_part_tab.addr, &ptr, 16);
		}
	}

	for (uint8_t i = 0; i < user_size; i++) {
		temp1 = cJSON_GetArrayItem(user_define, i);
		temp2 = cJSON_GetObjectItem(temp1, K_PARTITION_TYPE);

		if (!strcmp(V_KV, temp2->valuestring)) {
			memcpy(&e_kv.type, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_START_ADDR);
			memcpy(&e_kv.addr, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_SIZE_KB);
			e_kv.size_KB = temp2->valueint;

			kv_part.type = PARTITION_TYPE_KV;
			kv_part.start_addr = strtol(e_kv.addr, &ptr, 16);
			kv_part.size = e_kv.size_KB * 1024;
			kv_part.crc32 = GetCrc32((uint8_t*)&kv_part, sizeof(partition_info_t) - sizeof(uint32_t));
			memcpy(buf_4k, &kv_part, sizeof(partition_info_t));
			buf_4k += sizeof(partition_info_t);
		}
		else if (!strcmp(V_SIMU_EEPROM, temp2->valuestring)) {
			memcpy(&e_simu_eeprom.type, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_START_ADDR);
			memcpy(&e_simu_eeprom.addr, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_SIZE_KB);
			e_simu_eeprom.size_KB = temp2->valueint;

			simu_eeprom_part.type = PARTITION_TYPE_SIMU_EEPROM;
			simu_eeprom_part.start_addr = strtol(e_simu_eeprom.addr, &ptr, 16);
			simu_eeprom_part.size = e_simu_eeprom.size_KB * 1024;
			simu_eeprom_part.crc32 = GetCrc32((uint8_t*)&simu_eeprom_part, sizeof(partition_info_t) - sizeof(uint32_t));
			memcpy(buf_4k, &simu_eeprom_part, sizeof(partition_info_t));
			buf_4k += sizeof(partition_info_t);
		}
		else if (!strcmp(V_APP, temp2->valuestring)) {
			memcpy(&e_app.type, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_START_ADDR);
			memcpy(&e_app.addr, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_SIZE_KB);
			e_app.size_KB = temp2->valueint;

			app_part.type = PARTITION_TYPE_APP;
			app_part.start_addr = strtol(e_app.addr, &ptr, 16);
			app_part.size = e_app.size_KB * 1024;
			app_part.crc32 = GetCrc32((uint8_t*)&app_part, sizeof(partition_info_t) - sizeof(uint32_t));
			memcpy(buf_4k, &app_part, sizeof(partition_info_t));
			buf_4k += sizeof(partition_info_t);
		}
		else if (!strcmp(V_OTA, temp2->valuestring)) {
			memcpy(&e_ota.type, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_START_ADDR);
			memcpy(&e_ota.addr, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_SIZE_KB);
			e_ota.size_KB = temp2->valueint;

			ota_part.type = PARTITION_TYPE_OTA;
			ota_part.start_addr = strtol(e_ota.addr, &ptr, 16);
			ota_part.size = e_ota.size_KB * 1024;
			ota_part.crc32 = GetCrc32((uint8_t*)&ota_part, sizeof(partition_info_t) - sizeof(uint32_t));
			memcpy(buf_4k, &ota_part, sizeof(partition_info_t));
			buf_4k += sizeof(partition_info_t);
		}
		else if (!strcmp(V_NVDS, temp2->valuestring)) {
			memcpy(&e_nvds.type, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_START_ADDR);
			memcpy(&e_nvds.addr, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_SIZE_KB);
			e_nvds.size_KB = temp2->valueint;

			nvds_part.type = PARTITION_TYPE_NVDS;
			nvds_part.start_addr = strtol(e_nvds.addr, &ptr, 16);
			nvds_part.size = e_nvds.size_KB * 1024;
			nvds_part.crc32 = GetCrc32((uint8_t*)&nvds_part, sizeof(partition_info_t) - sizeof(uint32_t));
			memcpy(buf_4k, &nvds_part, sizeof(partition_info_t));
			buf_4k += sizeof(partition_info_t);
		}
		else if (!strcmp(V_USER, temp2->valuestring)) {
			memcpy(&e_user.type, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_START_ADDR);
			memcpy(&e_user.addr, temp2->valuestring, strlen(temp2->valuestring));
			temp2 = cJSON_GetObjectItem(temp1, K_SIZE_KB);
			e_user.size_KB = temp2->valueint;

			user_part.type = PARTITION_TYPE_USER;
			user_part.start_addr = strtol(e_user.addr, &ptr, 16);
			user_part.size = e_user.size_KB * 1024;
			user_part.crc32 = GetCrc32((uint8_t*)&user_part, sizeof(partition_info_t) - sizeof(uint32_t));
			memcpy(buf_4k, &user_part, sizeof(partition_info_t));
			buf_4k += sizeof(partition_info_t);
		}
	}
	//
	memset(buf_4k, 0, sizeof(partition_info_t));

	return true;
}

int app_proc(app_proc_ctx_t * app_proc_ctx)
{
	uint32_t boot_fsize = 0, app_fsize = 0, flashimage_fsize = 0, rd_len = 0, i = 0;
	char* buffer_boot = NULL;
	char* buffer_app = NULL;
	char* buffer_4k = NULL;
	uint32_t crc_result = { 0, };
	boot_header_t* boot_header = NULL;
	uint8_t ver_major, ver_minor, swd_crp;

	ver_major = app_proc_ctx->ver_major;
	ver_minor = app_proc_ctx->ver_minor;
	swd_crp = app_proc_ctx->swd_crp;

	fp_boot = app_proc_ctx->fp_boot;
	fp_app = app_proc_ctx->fp_app;
	fp_asm = app_proc_ctx->fp_asm;
	fp_flashimage = app_proc_ctx->fp_flashimage;
	fp_json = app_proc_ctx->fp_json;

	//build partition table
	if (NULL == (buffer_4k = (char*)malloc(SIZE_4KB))) {
		printf("Malloc memory fail.\n");
		return -1;
	}
	memset(buffer_4k, 0xFF, SIZE_4KB);
	if (true != build_partition_table(fp_json, buffer_4k)) {
		return -1;
	};

	//get file size
	fseek(fp_boot, 0, SEEK_END);
	boot_fsize = ftell(fp_boot);
	rewind(fp_boot);
	if (boot_fsize > boot_partition_size) {
		printf("boot_ram.bin is too large, file_size = %d, file_limit = %d\n", boot_fsize, boot_partition_size);
		return -1;
	}

	//get app file size
	fseek(fp_app, 0, SEEK_END);
	app_fsize = ftell(fp_app);
	rewind(fp_app);
	if (app_fsize > app_part.size) {
		printf("app.bin is too large, file_size = %d, file_limit = %d\n", app_fsize, app_part.size);
		return -1;
	}

	//read boot_ram.bin
	if (NULL == (buffer_boot = (char*)malloc(boot_partition_size))) {
		printf("Malloc memory fail.\n");
		return -1;
	}
	memset(buffer_boot, 0xFF, boot_partition_size);

	fseek(fp_boot, 0, SEEK_SET);
	if (boot_fsize != (rd_len = fread(buffer_boot, 1, boot_fsize, fp_boot))) {
		printf("Read boot_ram.bin error\n");
		free(buffer_boot);
		return -1;
	}

	//update swd enable(crp_flag)
	boot_header = (boot_header_t*)buffer_boot;
	if (swd_crp == 0) {
		boot_header->crp_flag = 0;
	} else {
		boot_header->crp_flag = CRP_VALID_FLAG;
	}

	//update boot hearder crc
	crc_result = GetCrc32((uint8_t*)boot_header, (sizeof(boot_header_t) - sizeof(uint32_t)));
	boot_header->boot_hearder_crc = crc_result;


	fseek(fp_flashimage, 0L, SEEK_SET);
	fwrite(buffer_boot, 1, boot_partition_size, fp_flashimage);
	fflush(fp_flashimage);

	free(buffer_boot);


	//write partition table
	fseek(fp_flashimage, partition_tab_base, SEEK_SET);
	fwrite(buffer_4k, 1, SIZE_4KB, fp_flashimage);

	//build KV space
    if((kv_part.size > 0) && (kv_part.start_addr < app_part.start_addr)){
        memset(buffer_4k, 0xFF, SIZE_4KB);
        for (i = 0; i < (kv_part.size / SIZE_4KB); i++) {
            fseek(fp_flashimage, kv_part.start_addr + i * SIZE_4KB, SEEK_SET);
            fwrite(buffer_4k, 1, SIZE_4KB, fp_flashimage);
        }
    }

	//build simu eeprom space
    if((simu_eeprom_part.size > 0) && (simu_eeprom_part.start_addr < app_part.start_addr)){
        memset(buffer_4k, 0xFF, SIZE_4KB);
        for (i = 0; i < (simu_eeprom_part.size / SIZE_4KB); i++) {
            fseek(fp_flashimage, simu_eeprom_part.start_addr + i * SIZE_4KB, SEEK_SET);
            fwrite(buffer_4k, 1, SIZE_4KB, fp_flashimage);
        }
    }

	//write app space
	if (NULL == (buffer_app = (char*)malloc(app_fsize))) {
		printf("Malloc memory fail.\n");
		free(buffer_4k);
		return -1;
	}
	memset(buffer_app, 0x00, app_fsize);

	fseek(fp_app, 0L, SEEK_SET);
	if (app_fsize != (rd_len = fread(buffer_app, 1L, app_fsize, fp_app))) {
		printf("read app.bin error\n");
		free(buffer_4k);
		free(buffer_app);
		return -1;
	}

	image_hdr_t image_header = { 0 };
	image_header.image_type = IMAGE_TYPE_ORIGINAL;
	image_header.ver.ver_major = ver_major;
	image_header.ver.ver_minor = ver_minor;
	image_header.img_size_orig = app_fsize;
	image_header.img_crc32_orig = 0;// GetCrc32(buffer_app, app_fsize);
	image_header.header_crc32 = 0;// GetCrc32((uint8_t*)&image_header, (sizeof(image_hdr_t) - sizeof(uint32_t)));

	fseek(fp_flashimage, app_part.start_addr, SEEK_SET);
	fwrite(&image_header, 1, sizeof(image_hdr_t), fp_flashimage);
	fwrite(buffer_app, 1, app_fsize, fp_flashimage);

	//fpu patch
	if (0 != fpu_patch(fp_asm, fp_flashimage))
	{
		free(buffer_4k);
		free(buffer_app);
		return -1;
	}

	//update image header crc32
	fseek(fp_flashimage, app_part.start_addr, SEEK_SET);
	if (sizeof(image_hdr_t) != (rd_len = fread(&image_header, 1L, sizeof(image_hdr_t), fp_flashimage))) {
		free(buffer_4k);
		free(buffer_app);
		return -1;
	}
	if (app_fsize != (rd_len = fread(buffer_app, 1L, app_fsize, fp_flashimage))) {
		free(buffer_4k);
		free(buffer_app);
		return -1;
	}
	image_header.img_crc32_orig = GetCrc32(buffer_app, app_fsize);
	printf("app size = %d\r\n", image_header.img_size_orig);
	printf("app crc32 = 0x%08x\r\n", image_header.img_crc32_orig);
	image_header.header_crc32 = GetCrc32((uint8_t*)&image_header, (sizeof(image_hdr_t) - sizeof(uint32_t)));

	fseek(fp_flashimage, app_part.start_addr, SEEK_SET);
	fwrite(&image_header, 1, sizeof(image_hdr_t), fp_flashimage);

	return 0;
}


bool app_version_parse(char* str, uint8_t* major, uint8_t* minor)
{
	int16_t len = 0;
	char* pos_start = str;
	char ver_str[4] = { 0, };

	//major
	pos_start = strstr(pos_start, "ver=");
	if (pos_start > 0) {
		pos_start += strlen("ver=");

		len = (int16_t)strcspn(pos_start, ".");
		if ((len < 1) || (len > (sizeof(ver_str) - 1))) {
			return false;
		}

		memset(ver_str, '\0', sizeof(ver_str));
		memcpy(ver_str, pos_start, len);

		*major = (uint8_t)atoi(ver_str);
	}
	else {
		return false;
	}

	//minor
	pos_start = strstr(pos_start, ".");
	if (pos_start > 0) {
		pos_start += sizeof(char);

		if (3 < strlen(pos_start)) {
			return false;
		}

		*minor = (uint8_t)atoi(pos_start);
	}
	else {
		return false;
	}

	return true;
}



