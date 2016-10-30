#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/select.h>

#include <jtl/time.h>
#include <jtl/fdops.h>

int jtl_write_nb(int fd, const void *data, size_t len, int timeout)
{
	int count = 0;
	uint32_t start = jtl_milli_uptime32(), wait = 1000;
	struct pollfd fds;

	fds.fd = fd;
	fds.events = POLLOUT;

	while (1) {
		int n;
		uint32_t past;

		n = write(fd, (const uint8_t *)data + count, len - count);
		if (n < 0) {
			if (errno != EAGAIN || errno != EWOULDBLOCK)
				break;
		} else {
			count += n;
		}
		if (count >= len)
			break;

		past = jtl_milli_uptime32() - start;

		if (timeout > 0) {
			if (past >= timeout)
				break;
			wait = timeout - past;
		}

		if (poll(&fds, 1, wait) <= 0)
			break;
	}

	return count;
}

int jtl_read_nb(int fd, void *buf, size_t size, int timeout)
{
	int count = 0;
	uint32_t start = jtl_milli_uptime32(), wait = 1000;
	struct pollfd fds;

	fds.fd = fd;
	fds.events = POLLIN;

	while (1) {
		int n;
		uint32_t past;

		n = read(fd, (uint8_t *)buf + count, size - count);
		if (n < 0) {
			if (errno != EAGAIN || errno != EWOULDBLOCK)
				break;
		} else {
			count += n;
		}
		if (count >= size)
			break;

		past = jtl_milli_uptime32() - start;

		if (timeout > 0) {
			if (count >0 || past >= timeout)
				break;
			wait = timeout - past;
		} else if(timeout <0) {
			if (past >= abs(timeout))
				break;
			wait = abs(timeout) - past;
        } else {
            if(count >0)
                break;
        }

		if (poll(&fds, 1, wait) <= 0)
			break;
	}

	return count;
}

