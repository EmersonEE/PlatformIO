#include "L298N.h"

L298N::L298N(gpio_num_t IN1, gpio_num_t IN2, gpio_num_t EN)
{
    this->IN1 = IN1;
    this->IN2 = IN2;
    this->EN = EN;
}

void L298N::motorConfig()
{
    ESP_LOGI(TAG, "Iniciando Configuracion De Pineres");
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << IN1) | (1ULL << IN2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    error = gpio_config(&io_conf);
    if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al configurar los GPIO IN1-IN2, %s", esp_err_to_name(error));
        return;
    }
    ESP_LOGI(TAG, "GPIO Configurado Correctamente");
}

void L298N::motorRight()
{
}
void L298N::motorLeft()
{
}

void L298N::motorForward()
{
}

void L298N::motorReverse()
{
}

void L298N::motorStop()
{
}
