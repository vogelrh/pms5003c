/**
 * C library for retrieving data from the Plantower PMS5003 particulate sensor.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "pms5003.h"

/**
 * Module level variables.
 */
int uart0_filestream = -1;
int uart_status = UART_NOT_INITIALIZED;
/**
 * ***** Private structs/unions and functions ******
 */

/**
 * Union for converting two byte reads from the UART to 
 * unsigned short int.
 * NOTE: This will ONLY work on Big Endian systems!!!
 * If using on Little Endian systems then the bytes must
 * be swapped before readin value;
 */
typedef union uart_word {
    uint16_t value;
    uint8_t data[2];
}uart_word;

/**
 * Union for converting the 26 bytes of data fread 
 * from the pms5003 to unsigned int values.
 * NOTE: Same Big Endian issues apply!
 */
typedef union pms5003_data_block {
    struct {
        uint16_t pm1cf;
        uint16_t pm2_5cf;
        uint16_t pm10cf;
        uint16_t pm1at;
        uint16_t pm2_5at;
        uint16_t pm10at;
        uint16_t gt0_3;
        uint16_t gt0_5;
        uint16_t gt1;
        uint16_t gt2_5;
        uint16_t gt5;
        uint16_t gt10;
        uint16_t reserved;
        uint16_t cksum;
    };
    uint8_t raw_data[PMS5003_EXPECTED_BYTES];
}pms5003_data_block;

/**
 * Reads two bytes from the UART and populates the specified uart_word
 * union.
 * 
 * param[out]       word        A uart_word union recieving the data.
 * 
 * Returns a UART status/error code.
 */
static int read_word(uart_word *word) {

    return UART_OK;
}

/**
 * Read a block of bytes from the PMS5003 and populates the specified
 * pms5003_data_block union. If the byte_cnt specified is not equal to 
 * PMS5003_EXPECTED_BYTES then a UART_UNEXPECTED_DATA error status is 
 * returned.
 * 
 * param[out]       data        A pms5003_data_block union recieving the data.
 * param[in]        byte_cnt    Byte count returned from reading the frame length word.
 * 
 * Returns a UART status/error code.
 */
static int read_pms_data_block(pms5003_data_block *data, int byte_cnt){


    return UART_OK;
}
/**
 * ***** Public Facing functions *****
 */
int uart_init(const char* device, int baud){
    char dev = PMS5003_DEV;
    if (device != NULL) {
        dev = device
    }

    uart0_filestream = open(dev, O_RDWR | ONOCTTY | O_NDELAY); //open in non blocking mode
    if (uart0_filestream == -1) {
        output_error(UART_INIT_ERROR);
        return UART_INIT_ERROR;
    }
    
    // Initialze for 8bit data, 1 stop bit, no parity no checkbit
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = baud | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
    return UART_OK;
}

int read_pms5003_data(pms5003_data *data){

    return UART_OK;
}

int read_pms5003_data(pms5003_data *data) {

    return UART_OK;
}

void output_error(int error_code) {

}