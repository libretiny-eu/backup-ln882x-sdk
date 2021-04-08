#ifndef __AIRKISS_DEF_H__
#define __AIRKISS_DEF_H__

// send `random` to mobile device via UDP broadcast port 10000.
#define AIRKISS_ACK_UDP_PORT    		(10000)
#define AIRKISS_ACK_UDP_IP      		("255.255.255.255")
#define AIRKISS_ACK_REPLY_CNT 			(20)

// receive `token` from mobile device via UDP server port 8266.
#define AIRKISS_TOKEN_RECV_PORT 		(8266)

#define AIRKISS_TOKEN_CMDTYPE_SET		(0) // mobile device -->   WiFi module
#define AIRKISS_TOKEN_CMDTYPE_REPLY 	(2)	// WiFi module   --> mobile device

// TODO: 适配不同的平台，长度需要调整
#define AIRKISS_TOKEN_MAX_LENGTH 		(100)

typedef struct airkiss_token {
	char token_string[AIRKISS_TOKEN_MAX_LENGTH];
	int  token_is_valid;
} airkiss_token_info_t;


#include "qcloud_iot_import.h"
#include "qcloud_iot_export.h"

// TODO: 适配不同的平台
#define XCLOUD_PLATFORM_PRODUCTID_LENGTH 	(MAX_SIZE_OF_PRODUCT_ID + 1) 	// 10+1
#define XCLOUD_PLATFORM_DEVNAME_LENGTH 		(MAX_SIZE_OF_DEVICE_NAME + 1) 	// 48+1
#define XCLOUD_PLATFORM_PRODUCTVER_LENGTH 	(10)

// TODO: 这个结构体也需要适配不同的平台
typedef struct xcloud_pltform_devinfo {
	char product_id[XCLOUD_PLATFORM_PRODUCTID_LENGTH];
	char device_name[XCLOUD_PLATFORM_DEVNAME_LENGTH];
	char product_ver[XCLOUD_PLATFORM_PRODUCTVER_LENGTH];
} xcloud_pltform_devinfo_t;

// TODO: adapt to different cloud platfrom.
#define XCLOUD_PLATFORM_BIND_TOKEN_REPLY_METHOD   	"app_bind_token_reply"
#define XCLOUD_PLATFORM_BIND_TOKEN_REPLY_CODE     	0
#define XCLOUD_PLATFORM_METHOD_BIND_DEVICE     		"bind_device"
#define XCLOUD_PLATFORM_TOKEN_REPORT_MAX 			(20)

typedef struct _bind_status
{
    int8_t bind_token_reply_cnt;
    int8_t bind_device_cnt;
    int8_t bind_is_ok;
} bind_status_t;


#endif // !__AIRKISS_DEF_H__
