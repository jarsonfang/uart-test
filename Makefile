CFLAGS += -Wall -g -O2 -I$(PWD)
CC = arm-linux-gcc

all:
	$(CC) $(CFLAGS) -o uart_test uart_test.c uart_test_main.c libpopt.a
	cp -v uart_test /tftpboot/

clean:
	rm -rf uart_test
	@echo "Done"
