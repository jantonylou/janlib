#ifndef __SERIAL_IO_H__
#define __SERIAL_IO_H__

#include <termios.h>
#include <unistd.h>

struct serial_port {
    const char *dev;
    int fd;

    int baud;

    uint8_t bf_rts_cts  :1; //使能硬件流控
    uint8_t bf_cstop_b  :1; //使用2-stop-bit
};

#define serial_port_init(port, i, b) do{  \
    memset(port, 0, sizeof(struct serial_port));    \
    (port)->fd    = -1; \
    (port)->iport = i;  \
    (port)->baud  = b;  \
}while(0)

int serial_open(struct serial_port *port);
int serial_write(struct serial_port *port, const void *data, int len);
int serial_read(struct serial_port *port, void *buf, int size, int usto);
#define serial_read_wait(port, usto) serial_read(port, NULL, 1, usto)
void serial_close(struct serial_port *port);

int serial_putb(struct serial_port *port, unsigned char b);

#endif

