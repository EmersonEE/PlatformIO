#include <Arduino.h>

#define LDR_PIN 34
#define LED_PWM 18


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin
#define SCREEN_ADDRESS 0x3C


// ledChannel = 0;
// frequency = 5000;
// resolution = 8;

#define LED_CHANNEL 0
#define FRECUENCY 5000
#define RESOLUTION 8