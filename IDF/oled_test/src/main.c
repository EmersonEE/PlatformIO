
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_sh1106.h"
#include "stdbool.h"
#include "xtensa/hal.h"

/* ================= CONFIG ================= */

#define I2C_MASTER_NUM I2C_NUM_0
#define SDA_GPIO 21
#define SCL_GPIO 22
#define OLED_ADDR 0x3C

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

/* ================= HANDLES ================= */

static esp_lcd_panel_handle_t panel_handle;
static i2c_master_bus_handle_t bus_handle;

/* ================= FRAMEBUFFER ================= */

static uint8_t buffer[OLED_WIDTH * OLED_HEIGHT / 8];

/* ================= FUENTE ================= */
// números 8x8
static const uint8_t font8x8_digits[10][8] = {
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},
    {0x0C, 0x1C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},
    {0x3E, 0x63, 0x03, 0x0E, 0x38, 0x60, 0x7F, 0x00},
    {0x3E, 0x63, 0x03, 0x1E, 0x03, 0x63, 0x3E, 0x00},
    {0x06, 0x0E, 0x1E, 0x36, 0x7F, 0x06, 0x06, 0x00},
    {0x7F, 0x60, 0x7E, 0x03, 0x03, 0x63, 0x3E, 0x00},
    {0x1E, 0x30, 0x60, 0x7E, 0x63, 0x63, 0x3E, 0x00},
    {0x7F, 0x63, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x00},
    {0x3E, 0x63, 0x63, 0x3E, 0x63, 0x63, 0x3E, 0x00},
    {0x3E, 0x63, 0x63, 0x3F, 0x03, 0x06, 0x3C, 0x00}};

/* ================= I2C INIT ================= */

static void i2c_master_init(void) {
  printf("Init I2C\n");

  i2c_master_bus_config_t bus_config = {
      .i2c_port = I2C_MASTER_NUM,
      .sda_io_num = SDA_GPIO,
      .scl_io_num = SCL_GPIO,
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .intr_priority = 0,
      .trans_queue_depth = 0,
  };

  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));
}

/* ================= DISPLAY INIT ================= */

static void sh1106_init(void) {
  printf("Creating panel IO...\n");

  esp_lcd_panel_io_handle_t io_handle = NULL;

  esp_lcd_panel_io_i2c_config_t io_config = {
      .dev_addr = OLED_ADDR,
      .control_phase_bytes = 1,
      .dc_bit_offset = 6,
      .scl_speed_hz = 100000,
  };

  ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(bus_handle, &io_config, &io_handle));

  printf("Creating panel driver...\n");

  esp_lcd_panel_dev_config_t panel_config = {
      .bits_per_pixel = 1,
      .reset_gpio_num = -1,
  };

  ESP_ERROR_CHECK(
      esp_lcd_new_panel_sh1106(io_handle, &panel_config, &panel_handle));

  printf("Init panel...\n");

  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

  // corrección de espejo horizontal
  ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, true));

  // offset típico del SH1106
  ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 2, 0));

  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

  printf("Display ON\n");
}

static void set_pixel(int x, int y, bool color) {
  if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT)
    return;

  int index = x + (y / 8) * OLED_WIDTH;

  if (color)
    buffer[index] |= (1 << (y % 8));
  else
    buffer[index] &= ~(1 << (y % 8));
}
/* ================= DRAW NUMBER ================= */

void draw_number_center(int num) {
  memset(buffer, 0x00, sizeof(buffer));

  char text[5];
  snprintf(text, sizeof(text), "%d", num);

  int len = strlen(text);

  int start_x = (OLED_WIDTH - (len * 8)) / 2;
  int start_y = (OLED_HEIGHT - 8) / 2;

  for (int i = 0; i < len; i++) {
    int digit = text[i] - '0';
    if (digit < 0 || digit > 9)
      continue;

    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {

        bool pixel_on = (font8x8_digits[digit][row] >> col) & 0x01;

        if (pixel_on) {
          set_pixel(start_x + i * 8 + row, start_y + col, true);
        }
      }
    }
  }

  ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, OLED_WIDTH,
                                            OLED_HEIGHT, buffer));

  printf("Numero mostrado: %d\n", num);
}
/* ================= MAIN ================= */

void app_main(void) {
  printf("APP START\n");

  i2c_master_init();
  sh1106_init();

  draw_number_center(99);
}
