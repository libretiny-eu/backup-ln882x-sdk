#ifndef __LN_CHIP_REBOOT_TRACE_H__
#define __LN_CHIP_REBOOT_TRACE_H__

#include "ln_types.h"

#define SOFTWARE_BOOT_MAGIC    (0x5AA5DCBA1234A55AULL)
#define WATCHDOG_BOOT_MAGIC    (0xA55A5678ABCD5AA5ULL)

typedef enum {
    CHIP_REBOOT_POWER_ON    = 0,
    CHIP_REBOOT_SOFTWARE    = 1,
    CHIP_REBOOT_WATCHDOG    = 2,
} chip_reboot_cause_t;

typedef struct {
    uint64_t        reboot_magic;         /*< reboot cause magic code */
    uint8_t         res_data[120];        /*< reserved no init data, customer planning */
} chip_no_init_data_t;

typedef enum
{
    WDT_TIMEOUT_LEVEL0      = 0,          /*< 8     ms @APBUS0_CLOCK=40MHz, 4     ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL1      = 1,          /*< 16    ms @APBUS0_CLOCK=40MHz, 8     ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL2      = 2,          /*< 32    ms @APBUS0_CLOCK=40MHz, 16    ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL3      = 3,          /*< 64    ms @APBUS0_CLOCK=40MHz, 32    ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL4      = 4,          /*< 128   ms @APBUS0_CLOCK=40MHz, 64    ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL5      = 5,          /*< 256   ms @APBUS0_CLOCK=40MHz, 128   ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL6      = 6,          /*< 512   ms @APBUS0_CLOCK=40MHz, 256   ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL7      = 7,          /*< 1024  ms @APBUS0_CLOCK=40MHz, 512   ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL8      = 8,          /*< 2048  ms @APBUS0_CLOCK=40MHz, 1024  ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL9      = 9,          /*< 4096  ms @APBUS0_CLOCK=40MHz, 2048  ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL10     = 10,         /*< 8192  ms @APBUS0_CLOCK=40MHz, 4096  ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL11     = 11,         /*< 16384 ms @APBUS0_CLOCK=40MHz, 8192  ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL12     = 12,         /*< 32768 ms @APBUS0_CLOCK=40MHz, 16384 ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL13     = 13,         /*< 65536 ms @APBUS0_CLOCK=40MHz, 32768 ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL14     = 14,         /*< 131072ms @APBUS0_CLOCK=40MHz, 65536 ms @APBUS0_CLOCK=80MHz */
    WDT_TIMEOUT_LEVEL15     = 15,         /*< 262144ms @APBUS0_CLOCK=40MHz, 131072ms @APBUS0_CLOCK=80MHz */ 
} watchdog_timeout_level_t;


/**
 * @brief get the cause of chip reboot.
 * get the cause of chip reboot.
 *
 * @attention 1. just called once. 
 *            2. call it in the first phase of the main function.
 *
 * @return  the cause of chip reboot.
 * @retval  #CHIP_REBOOT_POWER_ON    reboot by power on.
 * @retval  #CHIP_REBOOT_SOFTWARE    reboot by software reset.
 * @retval  #CHIP_REBOOT_WATCHDOG    reboot by watchdog timeout.
 */
chip_reboot_cause_t  ln_chip_get_reboot_cause(void);

/**
 * @brief chip software reboot.
 * it will reset all the hardware module except RAM.
 *
 * @attention None.
 *
 * @return  none
 */
void  ln_chip_reboot(void);

/**
 * @brief start watchdog on chip.
 * initialize watchdog,start it!
 *
 * @attention None.
 * 
 * @param[in]    level    indicating the watchdog timeout time, see WDT_TIMEOUT_LEVEL*.
 *
 * @return  none
 */
void  ln_chip_watchdog_start(watchdog_timeout_level_t level);

/**
 * @brief chip watchdog keepalive.
 * it will reset watchdog counter.
 *
 * @attention It must be called before the watchdog timeout.
 *
 * @return  none
 */
void  ln_chip_watchdog_keepalive(void);

#endif /* __LN_CHIP_REBOOT_TRACE_H__ */

