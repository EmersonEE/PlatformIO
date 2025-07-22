#include <RGB.h>

/// @brief Constructor para la clase RGB.
///
/// Inicializa un objeto RGB configurando los pines GPIO para los LEDs rojo, verde y azul.
/// @param _pinRed El número de pin GPIO al que está conectado el LED rojo.
/// @param _pinGreen El número de pin GPIO al que está conectado el LED verde.
/// @param _pinBlue El número de pin GPIO al que está conectado el LED azul.
RGB::RGB(uint8_t _pinRed, uint8_t _pinGreen, uint8_t _pinBlue)
{
    pinRed = _pinRed;
    pinGreen = _pinGreen;
    pinBlue = _pinBlue;
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false};
    ledc_timer_config(&ledc_timer);
    ledc_channel_config_t ledc_channel_reed = {
        .gpio_num = pinRed,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0};
    ledc_channel_config(&ledc_channel_reed);
    ledc_channel_config_t ledc_channel_green = {
        .gpio_num = pinGreen,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0};
    ledc_channel_config(&ledc_channel_green);
    ledc_channel_config_t ledc_channel_blue = {
        .gpio_num = pinBlue,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_2,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0};
    ledc_channel_config(&ledc_channel_blue);
}

/// @brief Establece el color del LED RGB usando valores PWM.
/// @param red Valor PWM para el canal rojo.
/// @param green Valor PWM para el canal verde.
/// @param blue Valor PWM para el canal azul.
void RGB::setColor(uint8_t red, uint8_t green, uint8_t blue)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, red);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, green);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, blue);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
}

/// @brief
/// @param sel
void RGB::seleColor(COLOR sel)
{
    switch (sel)
    {
    case COLOR::RED:
        RGB::setColor(255, 0, 0);
        break;
    case COLOR::GREEN:
        RGB::setColor(0, 255, 0);
        break;
    case COLOR::BLUE:
        RGB::setColor(0, 0, 255);
        break;
    case COLOR::PINK:
        RGB::setColor(255, 0, 128);
        break;
    case COLOR::VIOLET:
        RGB::setColor(183, 43, 226);
        break;
    case COLOR::ORANGE:
        RGB::setColor(255, 165, 26);
        break;
    case COLOR::YELLOW:
        RGB::setColor(255, 223, 0);
        break;
    case COLOR::GRIS:
        RGB::setColor(128, 128, 128);
        break;
    default:
        break;
    }
}

void RGB::randomColor()
{
    // Inicializa la semilla solo una vez
    static bool seeded = false;
    if (!seeded)
    {
        srand(time(NULL));
        seeded = true;
    }
    uint8_t red = rand() % 256;
    uint8_t green = rand() % 256;
    uint8_t blue = rand() % 256;
    setColor(red, green, blue);
}

void RGB::ledOff()
{
    RGB::setColor(0, 0, 0);
}