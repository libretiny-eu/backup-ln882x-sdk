#include "airkiss_port.h"
#include "osal/osal.h"
#include "wifi/wifi.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "utils/debug/log.h"
#include "utils/cJSON.h"
#include <stdarg.h>

static airkiss_token_info_t sg_airkiss_token = { 0 };

static xcloud_pltform_devinfo_t g_devinfo = { 0 };

static     OS_Timer_t    channel_hop_timer;

// TODO: adapt to different OS
cJSON_Hooks cjson_hooks = {
	.malloc_fn = pvPortMalloc,
	.free_fn = vPortFree
};

/**
 * @brief cJSON use OS malloc/free instead of stdanrd lib function.
 *
 */
void ak_port_init(void)
{
	cJSON_InitHooks(&cjson_hooks);
}

int ak_port_printf(const char* format, ...)
{
    #define PRINTF_BUF_LEN  (128)
    static char printf_buf[PRINTF_BUF_LEN];
    va_list args;

    va_start(args, format);
    vsnprintf(printf_buf, PRINTF_BUF_LEN, format, args);
    va_end(args);
    if(log_stdio_write){
        log_stdio_write(printf_buf, strlen(printf_buf));
    }
    return 0;
}

void ak_port_wifi_set_channel(uint8_t ch)
{
    wifi_set_channel(ch);
}

void ak_port_wifi_channel_hop_timer_creat(void * timer_cb, uint16_t * channel_mask)
{
    OS_TimerCreate(&channel_hop_timer, OS_TIMER_PERIODIC, (OS_TimerCallback_t)timer_cb, channel_mask, CHANNEL_HOP_INTERVAL);
}

void ak_port_wifi_channel_hop_start(void)
{
    OS_TimerStart(&channel_hop_timer);
}

void ak_port_wifi_channel_hop_stop(void)
{
    OS_TimerStop(&channel_hop_timer);
}

void ak_port_wifi_channel_hop_timer_del(void)
{
    OS_TimerDelete(&channel_hop_timer);
}

void ak_port_wifi_sniffer_enable(void * callback)
{
    wifi_set_promiscuous_filter(WIFI_PROMIS_FILTER_MASK_DATA);
    wifi_set_promiscuous_rx_cb((wifi_promiscuous_cb_t)callback);
    wifi_set_promiscuous(true);
}

void ak_port_wifi_sniffer_disable(void)
{
    wifi_set_promiscuous(false);
    wifi_set_promiscuous_rx_cb(NULL);
}

/**
 * @brief After connect to WiFi, this device send \em random via UDP broadcast
 * port 10000 to mobile device.
 *
 * @param random
 * @return int return 1 on success, 0 on failure.
 */
int ak_port_udp_send_ack(uint8_t random)
{
	uint8_t send_data = 0;
	int optval = 0;
	int ret = 0;

	struct sockaddr_in addr;
	int socketfd;

	send_data = random;

	if (0 > (socketfd = socket(AF_INET, SOCK_DGRAM, 0))) {
		ak_port_printf("%s() create socket fail\n", __func__);
		return AK_FALSE;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_port   = htons(AIRKISS_ACK_UDP_PORT);
	addr.sin_family = AF_INET;
	if (0 > inet_aton(AIRKISS_ACK_UDP_IP, &addr.sin_addr)) {
		ak_port_printf("%s() inet_aton error\n", __func__);
		goto err;
	}

	if (0 > setsockopt(socketfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int))) {
		ak_port_printf("%s() setsockopt error\n", __func__);
		goto err;
	}

	for (uint8_t i = 0; i < AIRKISS_ACK_REPLY_CNT; i++) {

		if (i < 2) { // show twice.
			ak_port_printf("airkiss reply random: 0X%02X\r\n", random);
		}

		ret = sendto(socketfd, &send_data, sizeof(send_data), 0, (struct sockaddr *)&addr, sizeof(addr));
		if (ret == -1) {
			ak_port_printf("%s() udp send error, %d\r\n", __func__);
			goto err;
		}
		OS_MsDelay(60);
	}

	closesocket(socketfd);
	return AK_TRUE;

err:
	closesocket(socketfd);
	return AK_FALSE;
}

__WEAK__  int HAL_GetDevInfo(void *pdevInfo)
{
	return AK_FALSE;
}

/**
 * @brief
 *
 * @param devinfo
 * @return int return AK_TRUE on success, AK_FALSE on failure.
 */
static int platform_get_devinfo(xcloud_pltform_devinfo_t *devinfo)
{
	if (NULL == devinfo) {
		ak_port_printf("%s | invalid parameter!\r\n", __func__);
		return AK_FALSE;
	}

	DeviceInfo pltDevInfo = { 0 }; // tencent qcloud platform
	if (QCLOUD_RET_SUCCESS != HAL_GetDevInfo((void *)&pltDevInfo)) {
		ak_port_printf("%s | HAL_GetDevInfo() failed!\r\n", __func__);
		return AK_FALSE;
	}

	memset(devinfo, 0, sizeof(xcloud_pltform_devinfo_t));
	memcpy(devinfo->product_id, pltDevInfo.product_id, strlen(pltDevInfo.product_id));
	memcpy(devinfo->device_name, pltDevInfo.device_name, strlen(pltDevInfo.device_name));
	memcpy(devinfo->product_ver, "0.1", strlen("0.1")); // FIXME: 还没有API获取产品版本，可以自行添加

	return AK_TRUE;
}


/**
 * @brief Parse token from `recvbuf` and save to `tokenbuf` if parse ok.
 *
 * @param recvbuf
 * @param tokenbuf
 * @param tokenbuflen
 * @return int return AK_TRUE on success, AK_FALSE on failure.
 */
static int parse_and_save_token(char *recvbuf, airkiss_token_info_t* token_info)
{
	if ((NULL == recvbuf) || (NULL == token_info)) {
		ak_port_printf("%s() invalid params!\r\n", __func__);
		return AK_FALSE;
	}

	cJSON *root = cJSON_Parse((const char *)recvbuf);
	if( NULL == root ) {
		ak_port_printf("%s() invalid cJSON string buf!!!\r\n", __func__);
		return AK_FALSE;
	}

	cJSON *cmdTypeObj = cJSON_GetObjectItem(root, "cmdType");
	cJSON *tokenObj = cJSON_GetObjectItem(root, "token");

	if ( ( NULL != cmdTypeObj) && (NULL != tokenObj)
		&& (cJSON_Number == cmdTypeObj->type) && (cmdTypeObj->valueint == AIRKISS_TOKEN_CMDTYPE_SET)
		&& (cJSON_String == tokenObj->type) ) {
			ak_port_printf("PARSE OK, token = [%s]\r\n", tokenObj->valuestring);
			memset(token_info, 0, sizeof(airkiss_token_info_t));
			memcpy(token_info->token_string, tokenObj->valuestring, strlen(tokenObj->valuestring));
			token_info->token_is_valid = 1;
	} else {
		ak_port_printf("%s() parse token failed!!!\r\n", __func__);
		cJSON_Delete(root);
		return AK_FALSE;
	}

	cJSON_Delete(root);
	return AK_TRUE;
}

/**
 * @brief 利用设备3元组构造 token 回复信息。
 *
 * @param pDevinfo 设备三元组
 * @return char*
 */
char * construct_token_reply_msg(xcloud_pltform_devinfo_t *pDevinfo)
{
	char *msg = NULL;

	cJSON* rootObj = cJSON_CreateObject();
	cJSON* cmdTypeObj = cJSON_CreateNumber(AIRKISS_TOKEN_CMDTYPE_REPLY);
	cJSON* productIdObj = cJSON_CreateString((const char *)pDevinfo->product_id);
	cJSON* deviceNameObj = cJSON_CreateString((const char *)pDevinfo->device_name);
	cJSON* productVerObj = cJSON_CreateString((const char *)pDevinfo->product_ver);

	if ((NULL != rootObj)
		&& (NULL != cmdTypeObj)
		&& (NULL != productIdObj)
		&& (NULL != deviceNameObj)
		&& (NULL != productVerObj)) {
		cJSON_AddItemToObject(rootObj, (const char *)"cmdType", cmdTypeObj);
		cJSON_AddItemToObject(rootObj, (const char *)"productId", productIdObj);
		cJSON_AddItemToObject(rootObj, (const char *)"deviceName", deviceNameObj);
		cJSON_AddItemToObject(rootObj, (const char *)"productVersion", productVerObj);
	}

	msg = cJSON_Print(rootObj);
	ak_port_printf("token reply msg: \r\n%s\r\n", msg);

	cJSON_Delete(rootObj);
	return msg;
}

void deconstruct_token_reply_msg(void *pMsg)
{
	if (pMsg) {
		OS_Free(pMsg);
	}
}

/**
 * @brief
 *
 * @param port
 * @return int
 */
int ak_port_recv_reply_token(void)
{
	int socket_fd = -1;
	struct sockaddr_in local, remote;
	socklen_t socklen = 0;

	struct timeval recv_timeout;
	recv_timeout.tv_sec = 1;
	recv_timeout.tv_usec = 0;

	int retry_cnt = 10;

	char temp_recv_buf[AIRKISS_TOKEN_MAX_LENGTH * 2] = { 0 };

	memset(&local, 0, sizeof(struct sockaddr_in));
	local.sin_len = sizeof(local);
	local.sin_family = AF_INET;
	local.sin_port = htons(AIRKISS_TOKEN_RECV_PORT);
	local.sin_addr.s_addr = htons(INADDR_ANY);

	if ( 0 > (socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) ) {
		ak_port_printf("%s() create socket fail.\r\n", __func__);
		return AK_FALSE;
	}

	if ( 0 != bind(socket_fd, (struct sockaddr *)&local, sizeof(struct sockaddr_in)) ) {
		ak_port_printf("%s() bind socket fail.\r\n", __func__);
		closesocket(socket_fd);
		return AK_FALSE;
	}

	if ( 0 != setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(struct timeval)) ) {
		ak_port_printf("%s() setsockopt SO_RCVTIMEO fail.\r\n", __func__);
		closesocket(socket_fd);
		return AK_FALSE;
	}

	while (retry_cnt-- > 0) {
		socklen = sizeof(struct sockaddr_in);
		int recv_ret = recvfrom(socket_fd, (void *)temp_recv_buf, sizeof(temp_recv_buf), 0, (struct sockaddr *)&remote, &socklen);
		if (recv_ret > 0) {
			// ak_port_printf("token recv string: [%s]\r\n", temp_recv_buf);
			if (AK_TRUE == parse_and_save_token(temp_recv_buf, &sg_airkiss_token)) {
				break;
			}
		}
	}

	if (retry_cnt <= 0) {
		closesocket(socket_fd);
		return AK_FALSE;
	}

	// 获取产品信息
	if ( AK_FALSE == platform_get_devinfo(&g_devinfo) ) {
		closesocket(socket_fd);
		return AK_FALSE;
	}

	// 构造 token 回复信息
	char *token_reply_msg = construct_token_reply_msg(&g_devinfo);
	if (NULL == token_reply_msg) {
		ak_port_printf("%s | token_reply_msg error!\r\n", __func__);
		closesocket(socket_fd);
		return AK_FALSE;
	}

	// reply token message to mobile device.
	for (retry_cnt = 0; retry_cnt < 10; retry_cnt++) {
		if (0 != sendto(socket_fd, token_reply_msg, strlen(token_reply_msg),
						0, (struct sockaddr *)&remote, socklen) ) {
			// no need to send if the mobile device has received this message and closed remote port.
			if (retry_cnt > 2 ) {
				// at least send 3 times.
				break;
			}
		}
	}
	deconstruct_token_reply_msg(token_reply_msg);

	closesocket(socket_fd);
	return AK_TRUE;
}

airkiss_token_info_t* ak_port_get_token(void)
{
	return &sg_airkiss_token;
}
