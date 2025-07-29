#include "MyADC.h"

MyADC::MyADC(adc_unit_t _adc_id,
             adc_bitwidth_t _bitwidth,
             adc_atten_t _atten,
             adc_channel_t _channel)
{
    atten = _atten;
    adc_id = _adc_id;
    bitwidth = _bitwidth;
    channel = _channel;
}

void MyADC::MyADC_Config()
{
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = adc_id,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = atten,
        .bitwidth = bitwidth,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, channel, &config));
}

int MyADC::MyADC_getRaw()
{
    int raw = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, channel, &raw));
    return raw;
}

MyADC::~MyADC()
{
    if (adc1_handle) {
        adc_oneshot_del_unit(adc1_handle);
    }
}
