#include <PantallitaXD.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// dame el codigo oxygen para cada funcion de PantallitaXD.h
/**
 * @brief Constructor de la clase PantallitaXD.
 */
PantallitaXD::PantallitaXD()
{
    // Constructor
}

/**
 * @brief Inicializa la pantalla OLED.
 * 
 * Debe llamarse antes de mostrar cualquier mensaje en la pantalla.
 */
void PantallitaXD::init()
{
    // Initialize the display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
}

/**
 * @brief Muestra un mensaje en la pantalla OLED.
 * 
 * Borra la pantalla y muestra el mensaje proporcionado en la posición (0,0).
 * 
 * @param message Mensaje a mostrar en la pantalla.
 */
void PantallitaXD::showMessage(const String message)
{
    // Show a message on the display
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(message);
    display.display();
}


/**
 * @brief Muestra el título "Carnet de Identidad" y el mensaje proporcionado en la pantalla.
 * 
 * El mensaje se muestra en tamaño de texto grande.
 * 
 * @param message Mensaje a mostrar como carnet.
 */
void PantallitaXD::showCarnet(const String message)
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(7, 0);
    display.setTextSize(1);
    display.println("Carnet de Identidad");
    display.setCursor(10, 20);
    display.setTextSize(2);
    display.println(message);
    display.display();
}


/**
 * @brief Muestra el título "Carnet de Identidad" y un mensaje de error en la pantalla.
 * 
 * El mensaje se muestra en tamaño de texto normal.
 * 
 * @param message Mensaje de error a mostrar.
 */
void PantallitaXD::showCarnetError(const String message)
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(7, 0);
    display.setTextSize(1);
    display.println("Carnet de Identidad");
    display.setCursor(0, 20);
    display.setTextSize(1);
    display.println(message);
    display.display();
}

void PantallitaXD::showAsistenciaYa(const String message)
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(7, 0);
    display.setTextSize(1);
    display.println("Carnet de Identidad");
    display.setCursor(8, 20);
    display.setTextSize(1);
    display.println(message);
    display.display();
}
