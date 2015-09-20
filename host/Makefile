CFLAGS += -Wall -g -O2 -I$(PWD)/../
LDFLAGS = -lpopt
CC = gcc

all:
	$(CC) -o uart_test ../uart_test.c ../uart_test_main.c $(CFLAGS) $(LDFLAGS) 

clean:
	rm -rf uart_test
	@echo "Done"
