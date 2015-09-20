#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <asm/ioctls.h>

#include "uart_test.h"

#define BUF_SIZE 256
static unsigned char buf[BUF_SIZE];

int tty_open_port(const char *dev_name)
{
	int fd;

	fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd)
	{
		perror("Open port failed" );
		return -1;
	}

//	printf("%s is opened\n", dev_name);

	if(fcntl(fd, F_SETFL, 0) < 0)
		printf("fcntl failed!\n");

	if (isatty(fd) == 0)
		perror("This is not a tty device");

//	printf("fd-open=%d\n", fd);
	return fd;
}

void tty_close_port(int fd)
{
	close(fd);
}

int tty_set_port(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios old_ios, new_ios;

	if (tcgetattr(fd, &old_ios) != 0)
	{
		perror("Save the terminal error");
		return -1;
	}

	bzero(&new_ios, sizeof(struct termios));

	switch (nSpeed)
	{
	case 1200:
		cfsetispeed(&new_ios , B1200);
		cfsetospeed(&new_ios , B1200);
		break;
	case 1800:
		cfsetispeed(&new_ios , B1800);
		cfsetospeed(&new_ios , B1800);
		break;
	case 2400:
		cfsetispeed(&new_ios , B2400);
		cfsetospeed(&new_ios , B2400);
		break;
	case 4800:
		cfsetispeed(&new_ios , B4800);
		cfsetospeed(&new_ios , B4800);
		break;
	case 9600:
		cfsetispeed(&new_ios , B9600);
		cfsetospeed(&new_ios , B9600);
		break;
	case 19200:
		cfsetispeed(&new_ios , B19200);
		cfsetospeed(&new_ios , B19200);
		break;
	case 38400:
		cfsetispeed(&new_ios , B38400);
		cfsetospeed(&new_ios , B38400);
		break;
	case 57600:
		cfsetispeed(&new_ios , B57600);
		cfsetospeed(&new_ios , B57600);
		break;
	case 115200:
		cfsetispeed(&new_ios , B115200);
		cfsetospeed(&new_ios , B115200);
		break;
	case 230400:
		cfsetispeed(&new_ios , B230400);
		cfsetospeed(&new_ios , B230400);
		break;
	case 460800:
		cfsetispeed(&new_ios , B460800);
		cfsetospeed(&new_ios , B460800);
		break;
	default:
		printf("Wrong  nSpeed\n");
//		break;
		return -1;
	}

	/* Clear data width mask bit */
	new_ios.c_cflag &= ~CSIZE;
	switch (nBits)
	{
	case 5:
		new_ios.c_cflag |=CS5 ;
		break ;
	case 6:
		new_ios.c_cflag |=CS6 ;
		break ;
	case 7:
		new_ios.c_cflag |=CS7 ;
		break ;
	case 8:
		new_ios.c_cflag |=CS8 ;
		break ;
	default:
		printf("Wrong  nBits\n");
//		break ;
		return -1;
	}

	switch (nEvent)
	{
	case 'o':
	case 'O':
		new_ios.c_cflag |= PARENB ;
		new_ios.c_cflag |= PARODD ;
		new_ios.c_iflag |= (INPCK | ISTRIP);
		break ;
	case 'e':
	case 'E':
		new_ios.c_iflag |= (INPCK | ISTRIP);
		new_ios.c_cflag |= PARENB ;
		new_ios.c_cflag &= ~PARODD ;
		break ;
	case 'n':
	case 'N':
		new_ios.c_cflag &= ~PARENB ;
		new_ios.c_iflag &= ~INPCK  ;
		break ;
	default:
		printf("Wrong nEvent\n");
//		break ;
		return -1;
	}

	switch (nStop)
	{
	case 1:
		new_ios.c_cflag &= ~CSTOPB;
		break;
	case 2:
		new_ios.c_cflag |= CSTOPB;
		break;
	default:
		printf("Wrong nStop\n");
//		break;
		return -1;
	}

	/* Enable the receiver and set local mode */
	new_ios.c_cflag |= CLOCAL |CREAD;

	/* No hardware control */
	new_ios.c_cflag &= ~CRTSCTS;

	/* No software control */
	new_ios.c_iflag &= ~(IXON | IXOFF | IXANY);

	/* raw model */
	new_ios.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);

	/* no output process */
	new_ios.c_oflag  &= ~OPOST;

	new_ios.c_iflag &= ~(INLCR|IGNCR|ICRNL);
	new_ios.c_oflag &= ~(ONLCR|OCRNL);

	/* min character */
	new_ios.c_cc[VMIN] = 0;

	/* time to wait for data */
	new_ios.c_cc[VTIME] = 0;

	tcflush(fd,TCIOFLUSH);

	if (tcsetattr(fd,TCSANOW,&new_ios) != 0)
	{
		perror("Set the terminal error");

		/* restore the old configuration */
		tcsetattr(fd,TCSANOW,&old_ios);
		return -1 ;
	}

	return 0;
}


void uart_test_read(uart_test_cb *p_cb)
{
	int i, nread;

	for (;;) {
		while (1) {
			bzero(buf, BUF_SIZE);
			nread = read(p_cb->fd, buf, BUF_SIZE);
			if (!nread) {
				usleep(1000); /* sleep 100ms */
				continue;
			}
			break;
		}

		printf("%s: ", __func__);
		for (i = 0; i < nread; i++) {
			printf("[%x] ", buf[i]);
		}

		putchar('\n');
	}
}

void uart_test_write(uart_test_cb *p_cb)
{
	int loop = p_cb->loop;
//	char *str = "\r\n";

	while (1) {
		if(strcmp(p_cb->type, "ascii")==0)
		{
			write(p_cb->fd, p_cb->str, strlen(p_cb->str));
			if (p_cb->verbose)
				printf("Write str: %s\n", p_cb->str);
		} else {
			write(p_cb->fd, &p_cb->hex, sizeof(p_cb->hex));
			if (p_cb->verbose)
				printf("Write data: %#x\n", p_cb->hex);
		}

//		write(p_cb->fd, str, strlen(str));

		sleep(1);
		if (!loop)
			continue;
		if(!(--loop))
			break;
	}
}

void uart_test_rw(uart_test_cb *p_cb)
{
	int ret, nbytes = 0;
	int nread, nwrite;
	int fd = p_cb->fd;
	int loop = p_cb->loop;
	struct timeval timeout;
	fd_set i_set;

	FD_ZERO(&i_set);
	FD_SET(fd, &i_set);

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	while (1) {
		/* write port */
		nwrite = write(fd, p_cb->str, strlen(p_cb->str));
		printf("%d bytes written\n", nwrite);

		/* read port */
		ret = select(fd+1, &i_set, NULL, NULL, &timeout);
		if (ret < 0)
			perror("select failed");
		else if (ret == 0)
			printf("timeout\n");
		else {
			if (FD_ISSET(fd, &i_set))
			{
				do {
					ioctl(fd, FIONREAD, &nbytes);
				} while (nbytes != nwrite);

				bzero(buf, BUF_SIZE);
				nread = read(fd, buf, nbytes);
				printf("nread=%d, str: %s\n", nread, buf);
			}
		}

		sleep(1);
		if (!loop)
			continue;
		if(!(--loop))
			break;
	}
}

#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0]))
#define TIOCWDT		_IOW('T', 0x60, unsigned int) /* initialize kernel feed dog timer */
#define TIOCNWDT	_IOW('T', 0x61, unsigned int) /* destroy kernel feed dog timer */
//#define TIOCNWDT 0x5461

static unsigned char queryWdt[] = {0x55,0x04,0x0a,0x0,0x01,0x36,0x01,0x20,0x66,0xaa};
static unsigned char openWdt[] = {0x55,0x04,0x0c,0x0,0x01,0x36,0x01,0x21,0x3c,0x01,0xa6,0xaa/*,
		0x55,0x04,0x0c,0x0,0x01,0x36,0x01,0x21,0x3c,0x02,0xa7,0xaa*/};
static unsigned char closeWdt[] = {0x55,0x04,0x0b,0x0,0x01,0x36,0x01,0x23,0x01,0x6b,0xaa/*,
		0x55,0x04,0x0b,0x0,0x01,0x36,0x01,0x23,0x02,0x6c,0xaa*/};

static int uart_test_nread(uart_test_cb *p_cb, void *buf, ssize_t len)
{
	int nread = 0;
	int loop = 50;
	unsigned char *pchar = buf;

	bzero(buf, len);

	do {
		int count;
		count = read(p_cb->fd, &pchar[nread], (len - nread));
		if (!count) {
			usleep(100000); /* sleep 100ms */
			if (!(--loop))
				return -1;
			continue;
		}
		if (count < 0)
			return -1;
		nread += count;
	} while (nread < len);

	return nread;
}

#define ACK_PKT_LEN 23
#define ACK_PKT_TYPE 0x20
#define DATA_OFFSET 7
void uart_test_queryWdt(uart_test_cb *p_cb)
{
	int i, nread;
	unsigned char *pchar;

	if (p_cb->queryWdt) {
		write(p_cb->fd, queryWdt, sizeof(queryWdt));
		printf("queryWdt: ");
		for (i=0; i < ARRAY_LEN(queryWdt); i++)
			printf("[%x] ", queryWdt[i]);
		putchar('\n');

		nread = uart_test_nread(p_cb, buf, ACK_PKT_LEN);
		if (nread < 0) {
			printf("Read watch dog state failed\n");
			return;
		}

		read(p_cb->fd, &buf[nread], (BUF_SIZE - ACK_PKT_LEN));	/* Read all of the remain data */

		if (ACK_PKT_TYPE != buf[DATA_OFFSET]) {
			printf("ACK message invalid, message type: %#x\n", buf[DATA_OFFSET]);
			return;
		}

		pchar = &buf[DATA_OFFSET];
		printf("Watch dog state:"
			   "a:%d/%d/%d %d:%d:%d "
			   "K:%d/%d/%d %d:%d:%d "
			   "ID:%d\n",
			   pchar[6], pchar[5], pchar[4], pchar[3], pchar[2], pchar[1],
			   pchar[12], pchar[11], pchar[10], pchar[9], pchar[8], pchar[7],
			   pchar[13]);
	}
}

void uart_test_openWdt(uart_test_cb *p_cb)
{
	int i;

	if (p_cb->openWdt) {
		write(p_cb->fd, openWdt, sizeof(openWdt));
		printf("openWdt: ");
		for (i=0; i < ARRAY_LEN(openWdt); i++)
			printf("[%x] ", openWdt[i]);
		putchar('\n');
	}
}

void uart_test_closeWdt(uart_test_cb *p_cb)
{
	int i;

	if(p_cb->closeWdt) {
		write(p_cb->fd, closeWdt, sizeof(closeWdt));
		printf("closeWdt: ");
		for (i=0; i < ARRAY_LEN(closeWdt); i++)
			printf("[%x] ", closeWdt[i]);
		putchar('\n');
	}
}

void uart_test_start_kwdt(uart_test_cb *p_cb)
{
	if (p_cb->start_kwdt) {
		printf("Start kernel watch dog\n");
		ioctl(p_cb->fd, TIOCWDT, &p_cb->time_interval);
	}
/*
	while (1) {
		printf("sleep 1s\n");
		sleep(1);
	}
*/
}

void uart_test_stop_kwdt(uart_test_cb *p_cb)
{
	if (p_cb->stop_kwdt) {
		printf("Stop kernel watch dog\n");
		ioctl(p_cb->fd, TIOCNWDT);
	}
}
