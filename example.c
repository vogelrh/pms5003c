/* testing */
#include <stdint.h>
#include <stdio.h>
#include "pms5003.h"



int main () {
    
    int status = pms_init();
    //printf("UART init status: %d\n", status);

    PMS5003_DATA d;

    int rstat = read_pms5003_data(&d);
    if (rstat != UART_OK) {
        output_uart_code(rstat);
    }

    printf("PM1.0   %d\nPM2.5   %d\nPM10    %d\nPM1.0a  %d\nPM2.5a  %d\nPM10a   %d\n>0.3    %d\n>0.5    %d\n>1      %d\n>2.5    %d\n>5      %d\n>10     %d\n",
    d.pm1cf,d.pm2_5cf,d.pm10cf,d.pm1at,d.pm2_5at,d.pm10at,d.gt0_3,d.gt0_5,d.gt1,d.gt2_5,d.gt5,d.gt10);
    pms_close();

    
}