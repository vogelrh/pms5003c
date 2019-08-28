/**
 * C library for retrieving data from the Plantower PMS5003 particulate sensor.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include "pms5003.h"

/**************************************************
 * Private structs/unions and constants
 **************************************************/
#define PMS5003_EXPECTED_BYTES 28
#define LITTLE_ENDED 1
static const uint16_t PMS5003_SOF = 19778; //the x42, x4d start characters

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
    }d;
    uint8_t raw_data[PMS5003_EXPECTED_BYTES];
}pms5003_data_block;

/**************************************************
 * Module level variables.
 **************************************************/
int uart0_filestream = -1;
int uart_status = UART_NOT_INITIALIZED;


/**************************************************
 * Private functions
 **************************************************/
/**
 * Swaps bytes in two byte words in byte array
 */
static void swap_bytes16(uint8_t *array) {
    uint8_t temp;
    int i;
    for (i = 0; i < sizeof(array); i = i + 2) {
        temp = array[i];
        array[i] = array[i+1];
        array[i+1] = temp;
    }
}
/**
 * Reads two bytes from the UART and populates the specified uart_word
 * union. Note because we are running in no-wait mode, we may not have 
 * any data yet. In that case we make sure the word structure has a
 * zero value.
 * 
 * param[out]       word        A uart_word union recieving the data.
 * 
 * Returns a UART status/error code.
 */
static int read_word(uart_word *word, int swap) {
    int rx_cnt = read(uart0_filestream,word->data, 2);
    if (rx_cnt < 0) {
        return UART_RX_ERROR; // some read error occured.
    } else if (rx_cnt ==0) { // no data yet.
        word->value = 0;
        //printf("no data waiting\n");
    } else {
        printf("rx_cnt %d\n",rx_cnt);
    }
    
    if (swap) {
        swap_bytes16(word->data);
    }
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
    int i;
    int checksum = 0;

    for (i=0; i < sizeof(data->raw_data);i++) {
        data->raw_data[i] = 0;
    }
   // get time at start an loop through reading 2 bytes from uart until
    // we find a start sequence or we exceed the timeout interval.
    clock_t begin;
    double elapse_time;

    begin = clock();
    uart_word sof;
 
    while(1) { //wait for SOF word
        elapse_time = (double)(clock() - begin) / CLOCKS_PER_SEC;
        if (elapse_time >= 5.0) {
            printf("Elapse time %0.3f", elapse_time);
            return UART_TIMEOUT_ERROR;
        }
        int status = read_word(&sof, 0);
        if (status == UART_OK && sof.value == PMS5003_SOF) {
            checksum = sof.data[0] + sof.data[1];
            break;
        }
    }
    printf("Have SOF\n");
    // Now read frame length word
    uart_word packet_length;
    int rstat = read_word(&packet_length, LITTLE_ENDED);

    if (packet_length.value != PMS5003_EXPECTED_BYTES) {
        return UART_UNEXPECTED_DATA_ERROR;
    }
    checksum += (packet_length.data[0] + packet_length.data[1]);
    printf("Have packet size");
    // Now read sensor data
    //int rx_cnt = read(uart0_filestream,data->raw_data, PMS5003_EXPECTED_BYTES);
    //if (rx_cnt < 0 ) { //|| rx_cnt != PMS5003_EXPECTED_BYTES) {
     //   return UART_RX_ERROR; // some read error occured.
    //}
    //hack..
    // Read until we have got all expected bytes - need timeout here

    uart_word w;
    for (i=0; i < packet_length.value; i+=2) {
        rstat = read_word(&w,0); 
        if (rstat != UART_OK) {
            return rstat;
        }
        data->raw_data[i] = w.data[i];
        data->raw_data[i+1] = w.data[i+1];
        //printf("reading %d\n",rx_cnt);
    }
    if (LITTLE_ENDED) {
        swap_bytes16(data->raw_data);
    }
        printf("expected %d %d\n",PMS5003_EXPECTED_BYTES, data->d.cksum );
        for(i=0; i < PMS5003_EXPECTED_BYTES; i++) {
            printf("[%d]\n", data->raw_data[i]);
        }
        printf("pm1.0 %d\npm2.5 %d\npm10 %d\n",data->d.pm1cf,data->d.pm2_5cf, data->d.pm10cf);
    for (i=0; i<PMS5003_EXPECTED_BYTES-2;i++) {
        checksum += data->raw_data[i];
    }
    if (checksum != data->d.cksum) {
        printf("Cksum: Expected: %d Got: %d",data->d.cksum,checksum);
        return UART_CHECKSUM_ERROR;
    }

    return UART_OK;
}

/**
 * Converts an integer value of the baud rate to the
 * correct constand value needed by the uart configuration.
 */
static int32_t convert_baudrate(int baudrate)
{
    switch(baudrate)
    {
        case 1200: return B1200;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
    }

    return -1;
}

/**************************************************
 * ***** Public Facing functions *****
 **************************************************/
int pms_init_override(char *device, int baud){
    if (device == NULL) {
        return UART_PARAMETER_ERROR;
    }

    int32_t baud_rate = convert_baudrate(baud);
    if (baud_rate < 0) {
        return UART_PARAMETER_ERROR;
    }

    uart0_filestream = open(device, O_RDWR | O_NOCTTY | O_NDELAY); //open in non blocking mode
    if (uart0_filestream == -1) {
        output_uart_code(UART_INIT_ERROR);
        return UART_INIT_ERROR;
    }
    
    // Initialze for 8bit data, 1 stop bit, no parity no checkbit
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    cfsetospeed(&options, baud_rate);
    cfsetispeed(&options, baud_rate);
    //no parity (8N1)
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    //Enable receiver and do not jange owner of port
    options.c_cflag |= CLOCAL | CREAD;

    //raw input
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	options.c_oflag = 0;
	//options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
    uart_status = UART_OK;
    return uart_status;
}

int pms_init() {
    return pms_init_override("/dev/serial0", 9600);
}

void pms_close() {
    close(uart0_filestream);
    uart_status = UART_NOT_INITIALIZED;
}

void output_uart_code(int error_code) {
    char *err_msg = NULL;
    switch(error_code) {
        case UART_OK:
            err_msg = "Status: OK";
            break;
        case UART_NO_DATA:
            err_msg = "Status: No data returned from UART";
            break;
        case UART_NOT_INITIALIZED:
            err_msg = "Status: UART not initialized";
            break;
        case UART_INIT_ERROR:
            err_msg = "ERROR: UART initialization error";
            break;
        case UART_PARAMETER_ERROR:
            err_msg = "ERROR: Incorrect UART parmeters provided";
            break;
        case UART_TX_ERROR:
            err_msg = "ERROR: UART transmit problem";
            break;
        case UART_RX_ERROR:
            err_msg = "ERROR: UART recieve problem";
            break;
        case UART_UNEXPECTED_DATA_ERROR:
            err_msg = "ERROR: PMS5003 unexpected data. Block size does not match data structure";
            break;
        case UART_CHECKSUM_ERROR:
            err_msg = "ERROR: PMS5003 data checksum error";
            break;
        case UART_TIMEOUT_ERROR:
            err_msg = "ERROR: PMS5003 read timeout error";
            break;
    }
    if (err_msg != NULL) {
        printf("%s\n", err_msg);
    }
}

int read_pms5003_data(PMS5003_DATA *data) {
    pms5003_data_block raw_data;
    int status = read_pms_data_block(&raw_data, PMS5003_EXPECTED_BYTES);
    if (status == UART_OK) {
        //transfer data to final structure
        return UART_OK;
    }
    return status;
}

