#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <popt.h>

#include "uart_test.h"

int uart_test_arg_parse(int argc, const char **argv, uart_test_cb *p_cb)
{
	memset(p_cb, 0, sizeof(*p_cb));

	//setting default values, popt will also use this to print help
	p_cb->port = strdup("/dev/ttySAC0");
	p_cb->type = strdup("hex");
	p_cb->str  = strdup("hello world!");
	p_cb->hex  = 0x55; /* quadrate waveform */
	p_cb->data = 8;
	p_cb->stop = 1;
	p_cb->baudrate = 9600;
	p_cb->parity = strdup("N");
	p_cb->time_interval = 2;

	struct poptOption option_table[] = {
			{"port", 'p', POPT_ARG_STRING|POPT_ARGFLAG_SHOW_DEFAULT, &p_cb->port,
					ARG_T_OPT_PORT, "tty device, serial port", NULL},
			{"type", 't', POPT_ARG_STRING|POPT_ARGFLAG_SHOW_DEFAULT, &p_cb->type,
					ARG_T_OPT_TYPE, "test type, ascii or hex", NULL},
			{"str", 's', POPT_ARG_STRING|POPT_ARGFLAG_SHOW_DEFAULT, &p_cb->str,
					ARG_T_OPT_STR, "string for ascii test", NULL},
			{"hex", 'H', POPT_ARG_INT|POPT_ARGFLAG_SHOW_DEFAULT, &p_cb->hex,
					ARG_T_OPT_HEX, "data for hex test", NULL},
			{"data", 'D', POPT_ARG_INT|POPT_ARGFLAG_SHOW_DEFAULT, &p_cb->data,
					ARG_T_OPT_DATA, "data bits, 5,6,7,8", NULL},
			{"stop", 'S', POPT_ARG_INT|POPT_ARGFLAG_SHOW_DEFAULT, &p_cb->stop,
					ARG_T_OPT_STOP, "stop bits, 1,2", NULL},
			{"parity", 'P', POPT_ARG_STRING|POPT_ARGFLAG_SHOW_DEFAULT, &p_cb->parity,
					ARG_T_OPT_PARITY, "parity, 'o'|'O', 'n'|'N', 'e'|'E'", NULL},
			{"baudrate", 'B', POPT_ARG_INT|POPT_ARGFLAG_SHOW_DEFAULT, &p_cb->baudrate,
					ARG_T_OPT_BAUDRATE, "port baudrate, 1200,1800,2400,4800,9600,19200,38400,57600,115200,230400,460800", NULL},
			{"loop", 'l', POPT_ARG_INT|POPT_ARGFLAG_SHOW_DEFAULT, &p_cb->loop,
					ARG_T_OPT_LOOP, "loop for write test, 0 for unlimited", NULL},
			{"read", 'r', POPT_ARG_NONE, &p_cb->read,
					ARG_T_OPT_READ, "read test", NULL },
			{"write", 'w', POPT_ARG_NONE, &p_cb->write,
					ARG_T_OPT_WRITE, "write test", NULL },
			{"echo", 'e', POPT_ARG_NONE, &p_cb->echo,
					ARG_T_OPT_WRITE, "echo test", NULL },
			{"queryWdt", '\0', POPT_ARG_NONE, &p_cb->queryWdt,
					ARG_T_OPT_WRITE, "query Wdt", NULL },
			{"openWdt", '\0', POPT_ARG_NONE, &p_cb->openWdt,
					ARG_T_OPT_WRITE, "set Wdt", NULL },
			{"closeWdt", '\0', POPT_ARG_NONE, &p_cb->closeWdt,
					ARG_T_OPT_WRITE, "close Wdt", NULL },
			{"start_kwdt", '\0', POPT_ARG_NONE, &p_cb->start_kwdt,
					ARG_T_OPT_WRITE, "start kernel dog timer", NULL },
			{"stop_kwdt", '\0', POPT_ARG_NONE, &p_cb->stop_kwdt,
					ARG_T_OPT_WRITE, "stop kenerl dog timer", NULL },
			{"time_interval", 'T', POPT_ARG_INT|POPT_ARGFLAG_SHOW_DEFAULT, &p_cb->time_interval,
					ARG_T_OPT_TIMER, "wdt timer interval", NULL},
			{"verbose", 'v', POPT_ARG_NONE, NULL,
					ARG_T_OPT_VERBOSE, "print verbose information", NULL},
			POPT_AUTOHELP
			POPT_TABLEEND
			};

	poptContext option_context = poptGetContext(argv[0], argc, argv, option_table, 0);

	if (argc < 2)
	{
		poptPrintUsage(option_context, stderr, 0);
		exit(-1);
	}

	int code;
	while ((code = poptGetNextOpt(option_context)) >= 0)
	{
		switch (code)
		{
		case ARG_T_OPT_VERBOSE:
			p_cb->verbose ++;
			break;
		}
	}

	if (code < -1) {
		printf("%s: %s\n",
				poptBadOption(option_context, POPT_BADOPTION_NOALIAS),
				poptStrerror(code));
		return -1;
	}

	/*
	 * check option values
	 */
	if (!p_cb->port)
	{
		printf("Invalid argruments.\n");
		return -1;
	}

	if(strcmp(p_cb->type, "ascii") != 0
			&& strcmp(p_cb->type, "hex") != 0)
	{
		printf("Invalid test type: %s\n", p_cb->type);
		return -1;
	}

	if (p_cb->read && p_cb->write)
	{
		printf("Invalid arguments: can't do both read and write test at the same time.");
		return -1;
	}

	return 0;
}

void uart_test_start(uart_test_cb *p_cb)
{
	int ret, fd;

	/*
	 * open port and set port options
	 */
	fd = tty_open_port(p_cb->port);
	if (fd < 0)
		return;
	
	p_cb->fd = fd;
	ret = tty_set_port(p_cb->fd, p_cb->baudrate,
				p_cb->data, p_cb->parity[0], p_cb->stop);
	if (ret < 0) {
		tty_close_port(p_cb->fd);
		return;
	}

	/*
	 * get start a test routine
	 */
	if (p_cb->queryWdt)
		uart_test_queryWdt(p_cb);
	else if (p_cb->openWdt)
		uart_test_openWdt(p_cb);
	else if (p_cb->closeWdt)
		uart_test_closeWdt(p_cb);
	else if (p_cb->start_kwdt)
		uart_test_start_kwdt(p_cb);
	else if (p_cb->stop_kwdt)
		uart_test_stop_kwdt(p_cb);
	else if (p_cb->read)
		uart_test_read(p_cb);
	else if (p_cb->write)
		uart_test_write(p_cb);
	else if (p_cb->echo)
		uart_test_rw(p_cb);
	else
		tty_close_port(p_cb->fd);
}

int main(int argc, char **argv)
{
	uart_test_cb arg;
	uart_test_cb* p_arg = &arg;

	memset(&arg, 0, sizeof(arg));

	if(uart_test_arg_parse(argc, (const char **)argv, p_arg) != 0)
		return -1;

	printf("running option\n"
			"\t tty  port:  %s\n"
			"\t data bits:  %u\n"
			"\t stop bits:  %u\n"
			"\t baudrate:   %u\n"
			"\t parity:     %c\n",
			p_arg->port,
			p_arg->data,
			p_arg->stop,
			p_arg->baudrate,
			p_arg->parity[0]
			);

	uart_test_start(p_arg);
	return 0;
}
