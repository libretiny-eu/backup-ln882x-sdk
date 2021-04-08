
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "boot_type.h"
#include "app_proc.h"


static FILE* fp_boot = NULL;
static FILE* fp_app = NULL;
static FILE* fp_asm = NULL;
static FILE* fp_flashimage = NULL;
static FILE* fp_json = NULL;
static app_proc_ctx_t app_proc_ctx = { 0 };

void usage(void)
{
	printf("LinghtningSemi LN882X image generate tool.\n");
}

extern int   boot_proc(FILE* fp_boot, uint8_t swd_en);
extern int   app_proc(app_proc_ctx_t* app_proc_ctx);
extern bool  app_version_parse(char* str, uint8_t* major, uint8_t* minor);

int main(int argc, char **argv)
{
	//argc = 9;

	//argv[1] = "cmd_app";
	//argv[2] = "F:\\ART2000-LN881x-LN882x\\ART2000-SRC-develop0423_ota\\project\\wifi_mcu_develop\\keil_ln882x\\boot_ln882x.bin";
	//argv[3] = "F:\\ART2000-LN881x-LN882x\\ART2000-SRC-develop0423_ota\\project\\wifi_mcu_develop\\keil_ln882x\\firmware_XIP.bin";
	//argv[4] = "F:\\ART2000-LN881x-LN882x\\ART2000-SRC-develop0423_ota\\project\\wifi_mcu_develop\\keil_ln882x\\firmware_XIP.asm";
	//argv[5] = "F:\\ART2000-LN881x-LN882x\\ART2000-SRC-develop0423_ota\\project\\wifi_mcu_develop\\keil_ln882x\\flashimage_dbg.bin";
	//argv[6] = "F:\\ART2000-LN881x-LN882x\\ART2000-SRC-develop0423_ota\\project\\wifi_mcu_develop\\keil_ln882x\\flash_partition_cfg.json";
	//argv[7] = "ver=1.0";
	//argv[8] = "swd_crp=0";

	char *cmd = argv[1];
	int i = 0;
	uint8_t swd_crp = 0;
	uint8_t ver_major = 1;
	uint8_t ver_minor = 0;

	if (argc < 2){
		usage();
		exit(-1);
	} else if (argc == 2 && strcmp(argv[1], "--help") == 0) {
		usage();
		exit(0);
	}

	//print all argv
	for (i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
	}

	/*----------------------------ln882x boot------------------------------------------*/
	//cmd_boot @L.bin swd=0
	if (strcmp(cmd, "cmd_boot") == 0)
	{
		if (argc < 3 || argc > 4) {
			printf("mkimage boot command arguments error!\n\n");
			usage();
			exit(-1);
		} else if (argc == 3){
			swd_crp = 0;
		} else if (argc == 4) {
			if (NULL != strstr(argv[3], "swd_crp=0")) {
				swd_crp = 0;
			} else if (NULL != strstr(argv[3], "swd_crp=1")) {
				swd_crp = 1;
			} else {
				swd_crp = 0;
			}
		}
		fp_boot = fopen(argv[2], "rb+");
		if(NULL == fp_boot){
			printf("无法打开文件:%s\n", argv[2]);
			exit(0);
		}

		boot_proc(fp_boot, swd_crp);
		fclose(fp_boot);
	}
	/*----------------------------ln882x app------------------------------------------*/
	//cmd_app ..\..\..\lib\boot_ln882x.bin @L.bin @L.asm flashimage.bin flash_partition_cfg.json ver=1.1 swd_crp=0
	else if (strcmp(cmd, "cmd_app") == 0)
	{
		if (argc < 8 || argc > 9) {
			printf("mkimage flashimage command arguments error!\n");
			usage();
			exit(-1);
		} else if (argc == 8) {
			swd_crp = 0;
		} else if (argc == 9) {
			if (NULL != strstr(argv[8], "swd_crp=0")) {
				swd_crp = 0;
			} else if (NULL != strstr(argv[8], "swd_crp=1")) {
				swd_crp = 1;
			} else {
				swd_crp = 0;
			}
		}
		fp_boot = fopen(argv[2],"rb");
		if(NULL == fp_boot) {
			printf("无法打开文件:%s\n", argv[2]);
			exit(-1);
		}
		fp_app = fopen(argv[3],"rb");
		if (NULL == fp_app) {
			printf("无法打开文件:%s\n", argv[3]);
			exit(-1);
		}
		fp_asm = fopen(argv[4],"rb");
		if (NULL == fp_asm) {
			printf("无法打开文件:%s\n", argv[4]);
			exit(-1);
		}
		fp_flashimage = fopen(argv[5], "wb+");
		if (NULL == fp_flashimage) {
			printf("无法打开文件:%s\n", argv[5]);
			exit(-1);
		}
		fp_json = fopen(argv[6], "rb");
		if (NULL == fp_json) {
			printf("无法打开文件:%s\n", argv[6]);
			exit(-1);
		}

		if (!app_version_parse(argv[7], &ver_major, &ver_minor)) {
			ver_major = 1;
			ver_minor = 0;
		}

		app_proc_ctx.fp_boot       = fp_boot;
		app_proc_ctx.fp_app        = fp_app;
		app_proc_ctx.fp_asm        = fp_asm;
		app_proc_ctx.fp_flashimage = fp_flashimage;
		app_proc_ctx.fp_json       = fp_json;

		app_proc_ctx.ver_major     = ver_major;
		app_proc_ctx.ver_minor     = ver_minor;
		app_proc_ctx.swd_crp       = swd_crp;

		app_proc(&app_proc_ctx);

		fclose(fp_boot);
		fclose(fp_app);
		fclose(fp_asm);
		fclose(fp_flashimage);
		fclose(fp_json);
	}
	else 
	{
		usage();
		exit(0);
	}

	return 0;
}





