
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Definición de pines con el tipo correcto
#define DIR_PIN GPIO_NUM_21    // Pin de dirección
#define STEP_PIN GPIO_NUM_22   // Pin de paso
#define ENABLE_PIN GPIO_NUM_23 // Pin de habilitación

// Configuración del motor
#define STEPS_PER_REV 200 // Pasos por revolución para un motor NEMA 17 típico
#define DELAY_US 500      // Retardo entre pasos en microsegundos

void setup_pins()
{
    // Configurar el pin de dirección
    gpio_config_t dir_conf = {
        .pin_bit_mask = (1ULL << DIR_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&dir_conf);

    // Configurar el pin de paso
    gpio_config_t step_conf = {
        .pin_bit_mask = (1ULL << STEP_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&step_conf);

    // Configurar el pin de habilitación
    gpio_config_t enable_conf = {
        .pin_bit_mask = (1ULL << ENABLE_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&enable_conf);

    // Inicialmente deshabilitar el motor (activo bajo en muchos drivers)
    gpio_set_level(ENABLE_PIN, 1);
}

void enable_motor()
{
    gpio_set_level(ENABLE_PIN, 0); // Habilitar motor (activo bajo)
    vTaskDelay(pdMS_TO_TICKS(10)); // Pequeño retardo para estabilización
}

void disable_motor()
{
    gpio_set_level(ENABLE_PIN, 1); // Deshabilitar motor
}

void rotate_motor(int steps, int direction)
{
    // Establecer dirección
    gpio_set_level(DIR_PIN, direction);
    vTaskDelay(pdMS_TO_TICKS(1)); // Pequeño retardo para que el pin de dirección se estabilice

    // Generar los pasos
    for (int i = 0; i < steps; i++)
    {
        gpio_set_level(STEP_PIN, 1);
        vTaskDelay(DELAY_US); // Retardo en alto
        gpio_set_level(STEP_PIN, 0);
        vTaskDelay(DELAY_US); // Retardo en bajo
    }
}

// Versión alternativa con control de velocidad variable
void rotate_motor_smooth(int steps, int direction, int speed_us)
{
    gpio_set_level(DIR_PIN, direction);
    vTaskDelay(pdMS_TO_TICKS(1));

    for (int i = 0; i < steps; i++)
    {
        gpio_set_level(STEP_PIN, 1);
        vTaskDelay(speed_us);
        gpio_set_level(STEP_PIN, 0);
        vTaskDelay(speed_us);
    }
}

void app_main(void)
{
    printf("Iniciando controlador de motor NEMA con TMC2209\n");

    // Configurar pines
    setup_pins();
    enable_motor();

    while (1)
    {
        printf("Girando a la izquierda (sentido antihorario)...\n");
        rotate_motor(STEPS_PER_REV, 0);  // 0 = izquierda/antihorario
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperar 1 segundo

        printf("Girando a la derecha (sentido horario)...\n");
        rotate_motor(STEPS_PER_REV, 1);  // 1 = derecha/horario
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperar 1 segundo

        // Versión con velocidad controlada
        printf("Media vuelta lenta a la izquierda...\n");
        rotate_motor_smooth(STEPS_PER_REV / 2, 0, 2000); // Más lento
        vTaskDelay(pdMS_TO_TICKS(2000));

        printf("Media vuelta rápida a la derecha...\n");
        rotate_motor_smooth(STEPS_PER_REV / 2, 1, 100); // Más rápido
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}