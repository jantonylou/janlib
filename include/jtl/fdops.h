#ifndef __JANLIB_FILE_OPERATIONS_H__
#define __JANLIB_FILE_OPERATIONS_H__

int jtl_write_nb(int fd, const void *data, size_t len, int timeout);
int jtl_read_nb(int fd, void *buf, size_t size, int timeout);

#endif
