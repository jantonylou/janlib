
CSRC = mo_buffer.c

CC = $(CROSS)gcc
AR = $(CROSS)ar

CFLAGS = -Wall -g -O2 -Iinclude

all:
	$(CC) $(CFLAGS) -c $(CSRC)
	$(AR) rv libjan.a *.o 

test:
	$(CC) $(CFLAGS) mo_buffer_test.c -ljan -L. -o test_mo_buffer && ./test_mo_buffer

clean:
	rm -fv *.o *.a test_*

