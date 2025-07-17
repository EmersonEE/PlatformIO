#include <stdio.h>
#include <esp_log.h>
extern "C"
{
#include <driver/i2c_master.h>
#include <esp_ssd1306.h>
}

extern "C" void app_main();
void app_main(void)
{

    static const uint8_t ssd1306_esp_logo_img[4][32] = {
        {0x00, 0x00, 0x00, 0xC0, 0x60, 0x18, 0x00, 0x00, 0x70, 0x78, 0x78, 0x78, 0xF8, 0xF8, 0xF0, 0xF0,
         0xF2, 0xE6, 0xE6, 0xCE, 0x9E, 0x9C, 0x3C, 0x78, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00},
        {0x00, 0xFC, 0x07, 0x60, 0xF8, 0xFC, 0xFE, 0xFE, 0x9E, 0x9E, 0x9E, 0x3E, 0x3E, 0x7C, 0x7C, 0xF9,
         0xF9, 0xF3, 0xE7, 0xCF, 0x9F, 0x3F, 0x7F, 0xFE, 0xFC, 0xF1, 0xE3, 0x8F, 0x1F, 0xFE, 0xF8, 0x00},
        {0x00, 0x07, 0x3C, 0xE0, 0x81, 0x03, 0x07, 0xC7, 0xE7, 0xC7, 0xCF, 0x1F, 0x7F, 0xFE, 0xFC, 0xF8,
         0xE1, 0x07, 0x3F, 0xFF, 0xFF, 0xFE, 0xF0, 0x01, 0x0F, 0xFF, 0xFF, 0xFF, 0x3C, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x06, 0x0D, 0x19, 0x11, 0x30, 0x20, 0x24, 0x4F, 0x4F, 0x4F,
         0x4F, 0x40, 0x40, 0x4F, 0x4F, 0x6F, 0x27, 0x20, 0x10, 0x10, 0x08, 0x0C, 0x04, 0x00, 0x00, 0x00},
    };

    /* I2C Master */
    static const i2c_master_bus_config_t i2c_master_bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
            .enable_internal_pullup = true}};
    static i2c_master_bus_handle_t i2c_master_bus;

    /* SSD1306 */
    static const i2c_ssd1306_config_t i2c_ssd1306_config = {
        .i2c_device_address = 0x3C,
        .i2c_scl_speed_hz = 400000,
        .width = 128,
        .height = 64,
        .wise = SSD1306_TOP_TO_BOTTOM};
    static i2c_ssd1306_handle_t i2c_ssd1306;
    while (true)
    {
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_bus_config, &i2c_master_bus));
        ESP_ERROR_CHECK(i2c_ssd1306_init(i2c_master_bus, &i2c_ssd1306_config, &i2c_ssd1306));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_image(&i2c_ssd1306, 48, 16, (const uint8_t *)ssd1306_esp_logo_img, 32, 32, false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_to_ram(&i2c_ssd1306));
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_fill(&i2c_ssd1306, false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_text(&i2c_ssd1306, 12, 0, "Hello, World!", false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_fill_space(&i2c_ssd1306, 0, 127, 8, 8, true));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, 10, "ABCDEFGHIJKLMNOP", false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, 18, "QRSTUVWXYZabcdfe", false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, 26, "ghijklmnopqrstuv", false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, 34, "wxyz1234567890!(", false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, 42, ")-=+[]{};:'\",.<>", false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, 50, "?/\\|_`~@#$%^&*", false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_fill_space(&i2c_ssd1306, 0, 127, 58, 63, true));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_to_ram(&i2c_ssd1306));
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_fill(&i2c_ssd1306, false));
        int x = 1234567890;
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_int(&i2c_ssd1306, 24, 0, x, false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_float(&i2c_ssd1306, 20, 8, x / 100000.0, 5, false));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_image(&i2c_ssd1306, 48, 20, (const uint8_t *)ssd1306_esp_logo_img, 32, 32, true));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_fill_space(&i2c_ssd1306, 0, 127, 58, 63, true));
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_ssd1306_buffer_to_ram(&i2c_ssd1306));
    }
}