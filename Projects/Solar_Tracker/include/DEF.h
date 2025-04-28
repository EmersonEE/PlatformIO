// Definición de pines LDR
#define LDR_SI 34 // Superior Izquierda
#define LDR_II 32 // Inferior Izquierda
#define LDR_SD 35 // Superior Derecha
#define LDR_ID 33 // Inferior Derecha

#define STEP 16
#define DIR 17
#define ENA 18

#define STEP_V 2
#define DIR_V 19
#define ENA_V 23

#define VRX_PIN 36 // ESP32 pin GPIO39 (ADC3) conectado al pin VRX
#define VRY_PIN 39 // ESP32 pin GPIO36 (ADC0) conectado al pin VRY
#define SW_PIN 4

#define LED_CONF 26
#define LED_LDR 25

#define MIN_STEP_X -360
#define MAX_STEP_X 360

#define MIN_STEP_Y 0
#define MAX_STEP_Y 300




// 24: ↑ (Flecha hacia arriba)
// 25: ↓ (Flecha hacia abajo)
// 26: → (Flecha hacia la derecha)
// 27: ← (Flecha hacia la izquierda)