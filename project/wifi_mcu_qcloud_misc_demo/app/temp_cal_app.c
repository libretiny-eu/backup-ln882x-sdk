#include "osal/osal.h"
#include "utils/debug/art_assert.h"
#include "utils/debug/log.h"
#include "wifi/wifi.h"
#include "hal/hal_adc.h"
#include "ln_kv_api.h"
#include "drv_adc_measure.h"

#define WIFI_TEMP_CALIBRATE          (1)

#if WIFI_TEMP_CALIBRATE
    static OS_Thread_t g_temp_cal_thread;
    #define TEMP_APP_TASK_STACK_SIZE   (4*256) //Byte
#endif // !WIFI_TEMP_CALIBRATE

void temp_cal_app_task_entry(void *params)
{
    int8_t cap_comp = 0;
    uint32_t v_len = 0;

    if (LN_TRUE == ln_kv_has_key(KV_XTAL_COMP_VAL)) {
        ln_kv_get(KV_XTAL_COMP_VAL,&cap_comp,sizeof(int8_t),&v_len);
    }
    drv_adc_init();
    OS_MsDelay(1);
    wifi_temp_cal_init(drv_adc_read(INTL_ADC_CHAN_0),cap_comp);

    while (1) {
        OS_MsDelay(1000);
        wifi_do_temp_cal_period(drv_adc_read(INTL_ADC_CHAN_0));
    }
}

void creat_temp_cal_app_task(void)
{
#if  WIFI_TEMP_CALIBRATE
    if(OS_OK != OS_ThreadCreate(&g_temp_cal_thread, "TempAPP", temp_cal_app_task_entry, NULL, OS_PRIORITY_BELOW_NORMAL, TEMP_APP_TASK_STACK_SIZE)) {
        ART_ASSERT(1);
    }
#endif
}
