/**
 * Header file for pms5003 library
 */
#include <stdint.h>
#include <stdio.h>
#include <termios.h>

#define UART_OK 0
#define UART_NO_DATA 1
#define UART_NOT_INITIALIZED 2
#define UART_INIT_ERROR 10
#define UART_PARAMETER_ERROR 11
#define UART_TX_ERROR 20
#define UART_RX_ERROR 21
#define UART_TIMEOUT_ERROR 22
#define UART_UNEXPECTED_DATA_ERROR 30
#define UART_CHECKSUM_ERROR 31


/**
 * Structure containing the pms5003 sensor data for a given reading.
 */
typedef struct pms5003_data {
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
}PMS5003_DATA;

/**
 * Structure for initializing UART for pms5003
 */
typedef struct pms5003_uart {
    char device; // uart device string. E.g. "/dev/serial0"
    int baud_rate; // baud rate integer as defined in terios.h (e.g. B9600)
}PMS5003_UART;

/**
 * Initializes and configures the UART for the PMS5003 using the devices
 * default parameters.
 * The UART is set up for device Serial0, 9600 baud, 8-bit, no parity, 
 * no check bit, one stop bit, read-write in a nonblocking mode.
 * 
 * Returns a UART status/error code.
 */
int pms_init();

/**
 * Initializes and configures the UART for the PMS5003 but allows the
 * user to specify the baud rate and serial device. 
 * The UART is set up for 8-bit, no parity, no check bit, one stop bit,
 * read-write in a nonblocking mode.
 * The default baud rate for the PMS5003 is 9600.
 * 
 * param[in]        device      UART device string, typically /dev/serial0
 * param[in]        baud        valid baud rate value, e.g. 1200, 2400, 9600
 * 
 * Returns a UART status/error code.
 */
int pms_init_override(char *device, int baud);

/**
 * Closes the UART connection to the PMS5003
 */
void pms_close();

/**
 * Reads the pms5003 and populates the specified pms5003_data structure.
 * 
 * param[out]       data        data stucture to populate.
 * 
 * Returns a UART status/error code.
 */
int read_pms5003_data(PMS5003_DATA *data);

/**
 * Outputs a specific error/ status message to STDOUT
 * 
 * param[in]        error_code      The UART function status/error code.
 */
void output_uart_code(int error_code);

