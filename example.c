/* testing */

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "pms5003.h"

#define ERROR_MAX 5

int main () {
    
    int status = pms_init();
    if (status != UART_OK) {
        output_uart_code(status);
        return 1;
    }

    int err_cnt = 0;
    PMS5003_DATA d;

    while (1) {
        status = read_pms5003_data(&d);
        if (status != UART_OK) {
            output_uart_code(status);
            err_cnt++;
            if (err_cnt > ERROR_MAX) {
                break;
            }
        }
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        printf("---- %d-%02d-%02d %02d:%02d:%02d ----\n",
                tm.tm_year + 1900,tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        printf("PM1.0   %d\nPM2.5   %d\nPM10    %d\nPM1.0a  %d\nPM2.5a  %d\nPM10a   %d\n>0.3    %d\n>0.5    %d\n>1      %d\n>2.5    %d\n>5      %d\n>10     %d\n",
                d.pm1cf,d.pm2_5cf,d.pm10cf,d.pm1at,d.pm2_5at,d.pm10at,d.gt0_3,d.gt0_5,d.gt1,d.gt2_5,d.gt5,d.gt10);
        sleep(1);
    }

    pms_close();
    return 0;
    
}