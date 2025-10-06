#include "MotorTMC.h"

/// @brief Constructor para la clase MotorTMC.
///
/// Inicializa una nueva instancia del motor TMC guardando los pines GPIO
/// asignados para las funciones de habilitación, paso (step) y dirección (direction).
///
/// @param _enPin El pin GPIO para la señal de Habilitación (Enable).
/// @param _stepPin El pin GPIO para la señal de Paso (Step).
/// @param _dirPin El pin GPIO para la señal de Dirección (Direction).
MotorTMC::MotorTMC(gpio_num_t _enPin, gpio_num_t _stepPin, gpio_num_t _dirPin)
{
    enPin = _enPin;
    stepPin = _stepPin;
    dirPin = _dirPin;
}

/// @brief Configura e inicializa los pines GPIO del motor.
///
/// Configura los pines de Habilitación, Paso y Dirección como salidas digitales.
/// Inmediatamente después de la configuración, establece el pin de habilitación (`enPin`)
/// a nivel **bajo** (0), lo que generalmente significa que el controlador del motor está **habilitado**
/// y listo para recibir comandos de paso.
void MotorTMC::init()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << enPin) | (1ULL << stepPin) | (1ULL << dirPin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);
    gpio_set_level(enPin, 0);
}

/// @brief Mueve el motor un paso en la dirección derecha/adelante.
///
/// Establece el pin de Dirección a **bajo** (0) y luego genera un pulso en el pin de Paso.
/// Cada llamada a esta función realiza dos ciclos de paso-y-espera, lo que resulta
/// en dos señales de paso enviadas al controlador.
void MotorTMC::movRight()
{
    gpio_set_level(dirPin, 0);
    gpio_set_level(stepPin, 1);
    vTaskDelay(pdMS_TO_TICKS(5));
    gpio_set_level(stepPin, 1);
    vTaskDelay(pdMS_TO_TICKS(5));
}

/// @brief Mueve el motor un paso en la dirección izquierda/atrás.
///
/// Establece el pin de Dirección a **alto** (1) y luego genera un pulso en el pin de Paso.
/// Cada llamada a esta función realiza dos ciclos de paso-y-espera, lo que resulta
/// en dos señales de paso enviadas al controlador.
void MotorTMC::movLeft()
{
    gpio_set_level(dirPin, 1);
    gpio_set_level(stepPin, 1);
    vTaskDelay(pdMS_TO_TICKS(5));
    gpio_set_level(stepPin, 1);
    vTaskDelay(pdMS_TO_TICKS(5));
}

/// @brief Realiza una rutina de prueba básica para el motor.
///
/// El motor se mueve 5 veces a la derecha y 5 veces a la izquierda,
/// con una pausa de 1 segundo (1000ms) entre cada cambio de dirección.
void MotorTMC::testMotor()
{
    for (int i = 0; i < 5; i++)
    {
        MotorTMC::movRight();
        vTaskDelay(pdMS_TO_TICKS(1000));
        MotorTMC::movLeft();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}