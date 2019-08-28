/* testing */
#include <stdint.h>
#include <stdio.h>
#include "pms5003.h"

typedef union uart_word {
    uint16_t value;
    uint8_t data[2];
}uart_word;

int main () {
    
    int status = pms_init();
    printf("UART init status: %d\n", status);

    PMS5003_DATA data;

    int rstat = read_pms5003_data(&data);
    pms_close();

    output_uart_code(rstat);
    uart_word uw;
    uw.data[0] = (uint8_t)28;
    uw.data[1] = (uint8_t)0;
    printf("uw.value: %hu\n", uw.value);
}