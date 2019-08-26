/* testing */
#include <stdint.h>
#include <stdio.h>
#include "pms5003.h"

int main () {
    
    int status = uart_init(PMS5003_DEV, PMS5003_BAUD);
    printf("UART init status: %d\n", status);
}