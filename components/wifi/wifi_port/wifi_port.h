#ifndef  __MACLIB_PORT_H__
#define  __MACLIB_PORT_H__

#include "ln_types.h"
#include "ln_wifi_err.h"

/* hardware for MAC Interrupt */
void    wlib_mac_isr_enable(void);
void    wlib_mac_isr_disable(void);

/* hardware for EVM test */
void    wlib_hwtimer_init(void * timer_cb, uint32_t period_us);
void    wlib_hwtimer_start(void);
void    wlib_hwtimer_stop(void);
void    wlib_software_reset_core(void);
void    wlib_pvtcmd_printf(const char *format, ...);
void    wlib_xtal40m_cap_set(uint8_t cap);

/* misc */
uint8_t wlib_char2hex(char c);
int     wlib_str2bytes(uint8_t *bytes, const char *str);
int     wlib_is_valid_mac_str(const char* mac_str);
int     wlib_is_valid_mac(const char *mac);
int     wlib_mac_str2hex(const uint8_t *str, uint8_t *hex);

/* restore/store psk info */
int    wlib_kv_has_psk_info_list(void);
int    wlib_kv_set_psk_info_list(uint8_t *v_buf, uint32_t buf_len);
int    wlib_kv_get_psk_info_list(uint8_t *v_buf, uint32_t buf_len);

/* tx power external compensation */
int    wlib_get_tx_power_ext_comp_val(int8_t *val);

/* heap memory manager */
void  *wlib_malloc(uint32_t size);
void   wlib_free(void *ptr);

/* host mem map, reg base, nop */
#include "ln88xx.h"
#define IMG_CAL_TMP_MEM_BASE   SRAM3_0_BASE
#define IMG_CAL_TMP_MEM_SIZE   SIZE_32KB

/* wlib log output */
void   wlib_log_printf(uint8_t tag_en, uint8_t level, const char *format, ...);
void   wlib_hexdump(const char *info, void *buf, uint32_t buf_len);
void   wlib_assert(int expr, const char *fun, int line);

#define WLIB_LOG_E_TAG            "[WLIB_E]"
#define WLIB_LOG_I_TAG            "[WLIB_I]"
#define WLIB_LOG_W_TAG            "[WLIB_W]"
#define WLIB_LOG_D_TAG            "[WLIB_D]"
#define WLIB_LOG_E                (1) //maclib log error output.
#define WLIB_LOG_I                (2) //maclib log (necessary) infor output.
#define WLIB_LOG_W                (3) //maclib log warning output.
#define WLIB_LOG_D                (4) //maclib log debug output.
#define WLIB_LOG_LEVEL_CFG        WLIB_LOG_D

#define WLIB_LOG(...)             wlib_log_printf(1, __VA_ARGS__)
#define WLIB_LOG_RAW(...)         wlib_log_printf(0, __VA_ARGS__)
#define WLIB_ASSERT(expr)         wlib_assert((int)expr, __func__, __LINE__)

/* cpu sleep voter register */
void   wlib_cpu_sleep_voter_reg(void *vote_check, void *pre_sleep, void *post_sleep);

/* sdio host interface */
void   wlib_sdio_if_init(void *wlib_recv_from_sdio, void **wlib_send_to_sdio, void *pkt_buf_malloc);
void   wlib_sdio_if_reset(void);

/* ssid support multi charset */
void   wlib_ssid_charset_match(const char *cfg_ssid, const char *target_ssid, char *out_data, uint32_t out_data_size);
void   wlib_ssid_gbk2utf8(char *ssid);


/* os service */
typedef void *wlib_thread_t;
typedef void *wlib_mutex_t;
typedef void *wlib_sem_t;
typedef void *wlib_queue_t;
typedef void *wlib_timer_t;

wlib_thread_t wlib_os_thread_creat(const char *name, void *entry, void *arg, uint32_t stksize, uint32_t prio);
void          wlib_os_thread_delete(wlib_thread_t *pthread);

wlib_queue_t  wlib_os_queue_creat(uint32_t item_count, uint32_t item_size);
void          wlib_os_queue_delete(wlib_queue_t *pqueue);
int           wlib_os_queue_send(wlib_queue_t *pqueue, const void *item, uint32_t timeout_ms);
int           wlib_os_queue_recv(wlib_queue_t *pqueue, void *item, uint32_t timeout_ms);

wlib_mutex_t  wlib_os_mutex_creat(void);
void          wlib_os_mutex_delete(wlib_mutex_t *pmutex);
int           wlib_os_mutex_lock(wlib_mutex_t *pmutex, uint32_t timeout_ms);
int           wlib_os_mutex_unlock(wlib_mutex_t *pmutex);

wlib_sem_t    wlib_os_sem_create(uint32_t max_count, uint32_t init_count);
void          wlib_os_sem_delete(wlib_sem_t *psem);
int           wlib_os_sem_wait(wlib_sem_t *psem, uint32_t timeout_ms);
int           wlib_os_sem_release(wlib_sem_t *psem);

wlib_timer_t  wlib_os_timer_creat(void *timer_cb, uint32_t data);
void          wlib_os_timer_delete(wlib_timer_t *ptimer);
void          wlib_os_timer_start(wlib_timer_t timer, uint32_t ms);
void          wlib_os_timer_stop(wlib_timer_t timer);
void          wlib_os_delay_ms(uint32_t ms);

#endif /* __MACLIB_PORT_H__ */

