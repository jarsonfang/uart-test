#ifndef UART_TEST_H
#define UART_TEST_H

typedef enum uart_test_arg_type_e {
	ARG_T_OPT_PORT,
	ARG_T_OPT_TYPE,
	ARG_T_OPT_STR,
	ARG_T_OPT_HEX,
	ARG_T_OPT_READ,
	ARG_T_OPT_WRITE,
	ARG_T_OPT_ECHO,
	ARG_T_OPT_DATA,
	ARG_T_OPT_STOP,
	ARG_T_OPT_PARITY,
	ARG_T_OPT_BAUDRATE,
	ARG_T_OPT_LOOP,
	ARG_T_OPT_TIMER,
	ARG_T_OPT_VERBOSE,
} uart_test_arg_type;

typedef struct uart_test_cb_s {
	/* test parameter */
	char *port;	// tty device, serial port 
	char *type;	// test type, ascii or hex
	char *str;	// string for ascii test
	char hex;	// data for hex test
	int  read;	// read test
	int  write;	// write test
	int  echo;	// echo test
	int  queryWdt;	// query hw wdt
	int  openWdt;	// set hw wdt
	int  closeWdt;	// close hw wdt
	int  start_kwdt;  // start kernel watchdog timer
	int  stop_kwdt;   // stop kernel watchdog timer
	int  time_interval;	// wdt timer interval
	int  data;	// data bits
	int  stop;	// stop bits
	char *parity;	// parity
	int  baudrate;	// port baudrate
	int  loop;	// loop for write test
	int  verbose;	// print verbose information

	/* runtime data */
	int  fd;	// file descriptor for the port
} uart_test_cb;

/*
 * Open a serial port
 * return the file fd if success, otherwise, return -1.
 */
int tty_open_port(const char *dev_name);

/* Close a serial port */
void tty_close_port(int fd);

/*
 * Set port option.
 * fd: file descriptor for the port, returned by tty_open_port()
 * nSpeed: baudrate, 1200,1800,2400,4800,9600,19200,38400,57600,115200,230400,460800
 * nBits: data bits, 5,6,7,8
 * nEvent: parity, 'o'|'O', 'n'|'N', 'e'|'E'
 * nStop: stop bits, 1,2
 * return 0 if success, -1 if error
 */
int tty_set_port(int fd, int nSpeed, int nBits, char nEvent, int nStop);

/*
 * test routines
 */
void uart_test_read(uart_test_cb *p_cb);
void uart_test_write(uart_test_cb *p_cb);
void uart_test_rw(uart_test_cb *p_cb);
void uart_test_queryWdt(uart_test_cb *p_cb);
void uart_test_openWdt(uart_test_cb *p_cb);
void uart_test_closeWdt(uart_test_cb *p_cb);
void uart_test_start_kwdt(uart_test_cb *p_cb);
void uart_test_stop_kwdt(uart_test_cb *p_cb);
#endif
