#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

int valor_adc = 0;
extern "C" void app_main();
void app_main()
{
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_5, &config));

    while (true)
    {
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_5, &valor_adc);
        printf("Valor leido: %d\n", valor_adc);
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    // Reiniciar la Unidad ADC
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
}