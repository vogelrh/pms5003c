/**
 * Header file for pms5003 library
 */
#include <stdint.h>
#include <stdio.h>
#include <termios.h>

#define PMS5003_SOF ((int16_t)19778) //the x42, x4d start characters
#define PMS5003_BAUD B9600
#define PMS5003_DEV "/dev/serial0"
#define PMS5003_EXPECTED_BYTES 28

#define UART_OK 0
#define UART_NO_DATA 1
#define UART_NOT_INITIALIZED 2
#define UART_INIT_ERROR 10
#define UART_TX_ERROR 11
#define UART_RX_ERROR 12
#define UART_UNEXPECTED_DATA_ERROR 13
#define UART_CHECKSUM_ERROR 14


/**
 * Structure containing the pms5003 sensor data for a given reading.
 */
typedef struct {
        uint16_t pm1cf;     // PM1.0 ug/m3 (ultrafine particles)
        uint16_t pm2_5cf;   // PM2.5 ug/m3 (combustion particles, organic compounds, metals)
        uint16_t pm10cf;    // PM10 ug/m3  (dust, pollen, mould spores)
        uint16_t pm1at;     // PM1.0 ug/m3 (atmos env)
        uint16_t pm2_5at;   // PM2.5 ug/m3 (atmos env)
        uint16_t pm10at;    // PM10 ug/m3 (atmos env)
        uint16_t gt0_3;     // >0.3um in 0.1L air
        uint16_t gt0_5;     // >0.5um in 0.1L air
        uint16_t gt1;       // >1.0um in 0.1L air
        uint16_t gt2_5;     // >2.5um in 0.1L air
        uint16_t gt5;       // >5.0um in 0.1L air
        uint16_t gt10;      // >10um in 0.1L air
} pms5003_data ;
/**
 * Initializes and configurs the UART for the PMS5003. 
 * The UART is set up for 8-bit, no parity, no check bit, one stop bit,
 * read-write in a nonblocking mode.
 * The default baud rate for the PMS5003 is 9600.
 * 
 * param[in]        device      UART device string, typically /dev/serial0
 * param[in]        baud        fcntl baud rate constant, e.g. B9600
 * 
 * Returns a UART status/error code.
 */
int uart_init(const char* device, int baud);

/**
 * Reads the pms5003 and populates the specified pms5003_data structure.
 * 
 * param[out]       data        data stucture to populate.
 * 
 * Returns a UART status/error code.
 */
int read_pms5003_data(pms5003_data *data);

/**
 * Outputs a specific error/ status message to stderr. Nothing will
 * be output if the code is UART_OK.
 * 
 * param[in]        error_code      The UART function status/error code.
 */
void output_error(int error_code);

