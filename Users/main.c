#include "Clock_service.h"
#include "Clock_display.h"
#include "Clock_mode.h"
#include "Encoder_service.h"
int main(){
    Display_init();
    Clock_Start();
    Encoder_service_init();
    key_service_init();
    while(1){
        Time_process();
        Display_update();
    }
}
