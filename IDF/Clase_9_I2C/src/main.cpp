#include <stdio.h>
#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern "C" void app_main();

#define I2C_SDA_PIN GPIO_NUM_21
#define I2C_SCL_PIN GPIO_NUM_22
#define I2C_PORT I2C_NUM_0

void i2c_scan()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
    };
    conf.master.clk_speed = 100000;
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
    printf("Escaneando Direcciones I2C...\n");

    for (int address = 0; address < 128; address++)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, 100 / portTICK_PERIOD_MS);
        if (ret == ESP_OK)
        {
            printf("Encontrado dispositivo en 0x%02X\n", address);
        }
        i2c_cmd_link_delete(cmd);
    }

    printf("ESCANEO TERMINADO....\n");
}
void app_main()
{

    while (true)
    {
        printf("Inciando Comunicacion I2C....\n");
        i2c_scan();
    }
}