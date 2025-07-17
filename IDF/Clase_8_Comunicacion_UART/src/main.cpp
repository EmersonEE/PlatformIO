#include <iostream>
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <esp_lcd_types.h>
#include <esp_lcd_panel_ssd1306.h>
// Configurar UART1 (GPIO17 para TX y GPIO16 para RX)
#define TX_PIN GPIO_NUM_17
#define RX_PIN GPIO_NUM_16
static const int RX_BUF_SIZE = 1024;

extern "C" void app_main();
void app_main()
{
    // Configurar UART1
    esp_err_t esp_lcd_new_panel_ssd1306();
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // Primero se configura la UART
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Luego se instala el driver
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);

    // Limpiar buffers
    uart_flush(UART_NUM_1);
    uart_flush_input(UART_NUM_1);

    ESP_LOGI("UART_APP", "UART1 configurado en TX:GPIO%d, RX:GPIO%d", TX_PIN, RX_PIN);

    int contador = 0;

    while (true)
    {
        char data[128];
        int length = uart_read_bytes(UART_NUM_1, data, sizeof(data) - 1, pdMS_TO_TICKS(100));

        if (length > 0)
        {
            data[length] = '\0';  // Terminar la cadena de texto correctamente

            std::cout << "Recibido en UART1: " << data << std::endl;

            if (strncmp(data, "hola", 4) == 0)
            {
                const char *mensaje = "Hola desde ESP32\n";
                uart_write_bytes(UART_NUM_1, mensaje, strlen(mensaje));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
