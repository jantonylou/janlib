#include <jtl/mo_packet.h>
#include <jtl/crc8.h>

struct mo_packet {
    uint8_t stat;
    uint8_t left;
    volatile uint8_t parsing;
};
#define MO_PACKET_PRIV_DATA_SIZE_NEED 3

#define MO_PACKET_HEAD_SIZE     2
#define MO_PACKET_PROTO_SIZE    3

#define MO_PACKET_MAGIC 0x7E

#if (MO_PACKET_PRIV_DATA_SIZE_NEED <=JTL_MO_BUFFER_PRIV_DATA)
#define PK_STAT_SYNC    0
#define PK_STAT_LENGTH  1
#define PK_STAT_DATA    2
#define PK_STAT_PARSE   3
#else
#error "JTL_MO_BUFFER_PRIV_DATA"
#endif

int mo_packet_putc(struct mo_buffer *mo, uint8_t c)
{
    int parse=0;
    struct mo_packet *pk = (void*)&mo->priv_data;

    if(mo_buffer_putc(mo, c) <0) goto _force_parse;

    if(pk->parsing || pk->stat==PK_STAT_PARSE) return 0;

    if(mo_buffer_get_space(mo) ==0) goto _force_parse;

    switch(pk->stat) {
        case PK_STAT_SYNC:
            if(c ==MO_PACKET_MAGIC) pk->stat = PK_STAT_LENGTH;
            break;
        case PK_STAT_LENGTH:
            pk->left = c;
            pk->stat = PK_STAT_DATA;
            break;
        case PK_STAT_DATA:
            if(pk->left-- ==0) {
                pk->stat = PK_STAT_PARSE;
                parse = 1;
            }
            break;

        default:
            pk->stat = PK_STAT_PARSE;
            parse = 1;
            break;
    }
    return parse;

_force_parse:
    if(pk->stat !=PK_STAT_PARSE) parse = 1;
    pk->stat = PK_STAT_PARSE;
    return parse;
}

#define PARSE_ABORT -1
#define PARSE_AGAIN -2
static int _do_packet_parse(struct mo_buffer *mo, void *buf, size_t size)
{
    struct mo_packet *pk = (void*)&mo->priv_data;
    int count;

    pk->parsing = 0xFF;

    for(;;) {
        uint8_t crc;
        int len, n, i;

        crc = 0;
        n = 0;
        pk->stat = PK_STAT_SYNC;
        count = mo_buffer_get_count(mo);

        // search sync char
        for(;;) {
            int c = mo_buffer_copy_first(mo);
            if(c <0) goto _parse_abort;
            if(c ==MO_PACKET_MAGIC) break;
            mo_buffer_drop_char(mo);
        }
        pk->stat = PK_STAT_LENGTH;
        n++;

        len = mo_buffer_copy_char(mo, n);
        if(len <0) goto _parse_abort;
        n++;
        pk->stat = PK_STAT_DATA;
        pk->left = 0;

        if((len+MO_PACKET_PROTO_SIZE) >mo_buffer_get_size(mo)) {
            mo_buffer_drop_char(mo);
            continue;
        }
        if((len+MO_PACKET_PROTO_SIZE) >count) {
            pk->left = len - (count-MO_PACKET_HEAD_SIZE);
            goto _parse_abort;
        }

        // calc crc8
        for(i=0; i<len; i++, n++) {
            crc = jtl_crc8_byte(mo_buffer_copy_char(mo, n), crc);
        }
        if(crc !=mo_buffer_copy_char(mo, n)) {
            mo_buffer_drop_char(mo);
            continue;
        }
        pk->stat = PK_STAT_SYNC;

        // drop packet len
        mo_buffer_drop(mo, MO_PACKET_HEAD_SIZE);

        // copy data
        if(size <len) {
            n = mo_buffer_read(mo, buf, size);
        } else {
            n = mo_buffer_read(mo, buf, len);
        }
        if(n <len) mo_buffer_drop(mo, len-n);
        mo_buffer_drop(mo, 1); // drop crc byte
        return n;
    }

_parse_abort:
    pk->parsing = 0;
    if(count != mo_buffer_get_count(mo)) return PARSE_AGAIN;
    return PARSE_ABORT;
}

int mo_packet_parse(struct mo_buffer *mo, void *buf, size_t size)
{
    int result;

    do {
        result = _do_packet_parse(mo, buf, size);
    } while(result ==PARSE_AGAIN);

    return result;
}

int mo_packet_push(struct mo_buffer *mo, const void *buf, int len)
{
    if((len+MO_PACKET_PROTO_SIZE) >mo_buffer_get_space(mo)) return -1;
    if(len <0) return -1;

    if(mo_buffer_putc(mo, MO_PACKET_MAGIC) <0) return -1;
    if(mo_buffer_putc(mo, len) <0) return -1;
    if(mo_buffer_write(mo, buf, len) !=len) return -1;
    if(mo_buffer_putc(mo, jtl_crc8(buf, len)) <0) return -1;

    return 0;
}

