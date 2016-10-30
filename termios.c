#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>  
#include <fcntl.h> 
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/time.h>

#include <jtl/debug.h>
#include <jtl/termios.h>

const struct { int baud; int cf; } baud_map[] = {
    {   9600, B9600   },
    {  57600, B57600  },
    { 115200, B115200 },
};

static int cf_port_baud(baud) {
    int i;

    for(i=0; ; i++) {
        if(baud_map[i].baud == baud) return baud_map[i].cf;
    }

    return B9600;
}

// sets = "|CRTSCTS|CSTOPB|"
int jtl_open_tty(const char *dev, int baud, const char *sets) {
    int fd;
    struct termios opt; 
    int flags;

    if(sets ==NULL) sets = "";

    flags = 0;

    flags |= O_RDWR;
    flags |= O_NOCTTY;
    flags |= O_NONBLOCK;

    DP_MSG("open serial port %s, baud rate %d", dev, baud);

    fd = open(dev, flags);
    if(fd <0) {
        perror("open serial port failed");
        return -1;
    }

    tcgetattr(fd, &opt); 

    cfsetispeed(&opt, cf_port_baud(baud));
    cfsetospeed(&opt, cf_port_baud(baud));
    
    opt.c_cflag &= ~CSIZE; 
    opt.c_cflag |= CS8;
    opt.c_cflag &= ~CSTOPB; 
    opt.c_cflag &= ~PARENB; 
    opt.c_cflag &= ~INPCK;
    opt.c_cflag |= (CLOCAL | CREAD);
    opt.c_cflag &= ~CRTSCTS;

    if(strstr(sets, "|CRTSCTS|")) opt.c_cflag |= CRTSCTS;
    if(strstr(sets, "|CSTOPB|")) opt.c_cflag |= CSTOPB;
 
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
 
    opt.c_oflag &= ~OPOST;
    opt.c_oflag &= ~(ONLCR | OCRNL);
 
    opt.c_iflag &= ~(ICRNL | INLCR);
    opt.c_iflag &= ~(IXON | IXOFF | IXANY);
    
    opt.c_cc[VTIME] = 0;
    opt.c_cc[VMIN] = 0;

    cfmakeraw(&opt);

    tcflush(fd, TCIOFLUSH);
 
    if(tcsetattr(fd, TCSANOW, &opt) != 0)
    {
        close(fd);
        return -1;
    }

    return fd;
}

