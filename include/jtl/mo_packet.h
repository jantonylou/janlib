#ifndef __JTL_SERIAL_PACKET_H__
#define __JTL_SERIAL_PACKET_H__

#include <jtl/mo_buffer.h>

// return 0: More data, >0: Packet ready (need parse), -1: error
int mo_packet_putc(struct mo_buffer *mo, uint8_t c);
// return >0: Packet length, -1: More data
int mo_packet_parse(struct mo_buffer *mo, void *buf, size_t size);

// write packet to buffer, return 0 for success, -1 for error
int mo_packet_push(struct mo_buffer *mo, const void *buf, int len);

#endif
