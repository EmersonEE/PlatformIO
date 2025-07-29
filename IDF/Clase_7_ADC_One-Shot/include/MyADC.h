#ifndef __MYADC_H__
#define __MYADC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

class MyADC
{
private:
    adc_atten_t atten;
    adc_unit_t adc_id;
    adc_bitwidth_t bitwidth;
    adc_channel_t channel;
    adc_oneshot_unit_handle_t adc1_handle;

public:
    MyADC(adc_unit_t _adc_id = ADC_UNIT_1,
          adc_bitwidth_t _bitwidth = ADC_BITWIDTH_DEFAULT,
          adc_atten_t _atten = ADC_ATTEN_DB_12,
          adc_channel_t _channel = ADC_CHANNEL_5);

    void MyADC_Config();
    int MyADC_getRaw();
    ~MyADC();
};

#endif // __MYADC_H__
