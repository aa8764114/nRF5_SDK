//這個練習是將程式內的運行狀況透過log輸出
//觀察運作狀態

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nordic_common.h"
#include "boards.h"

//匯入跟log相關的函數
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//匯入延遲函數
#include "nrf_delay.h"

int main(void)
{
    //初始化log函數
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    
    //輸出log->info訊息（已經先在config中說要印log->info了）
    NRF_LOG_INFO("This is log data from nordic!!")
    
    //設定計數初始值
    int count = 0;
    while (true)
    {
        NRF_LOG_INFO("Count : %d", count);//透過log->info印出計數值
        nrf_delay_ms(500);//延遲500毫秒
        count ++;//計數值+1
    }
}

