#include <MotorTMC.h>

extern "C" void app_main();
void app_main()
{

    MotorTMC Nema1(GPIO_NUM_23, GPIO_NUM_22, GPIO_NUM_21);
    Nema1.init();
    while (true)
    {
        Nema1.movLeft();
    }
}