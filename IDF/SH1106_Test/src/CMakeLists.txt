# CMakeLists.txt (en la raíz del proyecto, al lado de platformio.ini)

cmake_minimum_required(VERSION 3.16)

# Opcional: si quieres forzar la carpeta src como componente principal
# (PlatformIO lo hace por defecto)

idf_component_register(
    SRCS 
        "src/main.c"          # agrega aquí todos tus .c / .cpp
        # "src/otro_archivo.c"
        # "src/utils/helper.c"
    INCLUDE_DIRS 
        "."                   # o "src/include" si tienes headers separados
    REQUIRES 
        esp_lcd
        sh1106-esp-idf
    # O PRIV_REQUIRES si no expones nada público
    # PRIV_REQUIRES esp_lcd sh1106-esp-idf
)
