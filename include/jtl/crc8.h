#ifndef __JANLIB_CRC_8BIT_H__
#define __JANLIB_CRC_8BIT_H__

#include <stdint.h>
#include <sys/types.h>

uint8_t jtl_crc8_part(const void *data, size_t len, uint8_t crc);
#define jtl_crc8(data, len) jtl_crc8_part(data, len, 0)
uint8_t jtl_crc8_byte(uint8_t b, uint8_t crc);

#endif
