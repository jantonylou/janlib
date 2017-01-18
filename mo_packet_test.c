#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <jtl/debug.h>
#include <jtl/mo_packet.h>
#include <jtl/crc8.h>

#define MO_PACKET_MAGIC 0x7E
#define MO_PACKET_PROTO_SIZE    3

int main(int argc, char *argv[]) {
    int i;
    struct mo_buffer mo;
    uint8_t buf[1024], pas[255];

    DP_MSG("mo_packet test start");

    mo_buffer_init(&mo, buf, sizeof(buf));

    DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0) >0);

    DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0) ==0);

    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) ==0);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) ==0);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);

    // crc error test
    DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 1) ==1);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);

    DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0) >0);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) ==0);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);

    DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0) ==0);
    DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==1);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);

    DP_ASSERT(mo_packet_putc(&mo, 0) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0) >0);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) ==0);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);

    // crc error test
    DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 1) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0x5A) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0) ==1);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);

    DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 1) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 0xBB) ==0);
    DP_ASSERT(mo_packet_putc(&mo, jtl_crc8_byte(0xBB, 0)) >0);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) ==1);
    DP_ASSERT(pas[0]==0xBB);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);

    DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 1) ==0);
    DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==0);
    DP_ASSERT(mo_packet_putc(&mo, 1) ==1);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);

    DP_ASSERT(mo_packet_putc(&mo, 0xAA) ==0);
    DP_ASSERT(mo_packet_putc(&mo, jtl_crc8_byte(0xAA, 0)) >0);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) ==1);
    DP_ASSERT(pas[0]==0xAA);
    DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);

    for(i=0; i<100000; i++) {
        int n;
        uint8_t crc=0, b=i;

        //DP_MSG("i=%d", i);

        DP_ASSERT(mo_packet_putc(&mo, MO_PACKET_MAGIC) ==0);
        DP_ASSERT(mo_packet_putc(&mo, i) ==0);
        for(n=0; n<b; n++) {
            DP_ASSERT(mo_packet_putc(&mo, b+n) ==0);
            crc = jtl_crc8_byte(crc, b+n);
        }
        DP_ASSERT(mo_packet_putc(&mo, crc) >0);

        DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) ==b);
        for(n=0; n<b; n++) {
            //DP_MSG("n=%d, b=%d", n, b);
            DP_ASSERT(pas[n] ==((uint8_t)(b+n)));
        }

        DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);
        DP_ASSERT(mo_buffer_get_count(&mo) ==0);
    }

    for(i=0; i<100000; i++) {
        const int bad_limit=i%600;
        int n;
        uint8_t crc=0, b=i;

        //DP_MSG("i=%d", i);

        for(n=0; n<bad_limit; n++) {
            b = n;
            if(b ==(MO_PACKET_MAGIC +1)) b = (bad_limit-n)/2;
            if(b ==(MO_PACKET_MAGIC +2)) b = 0;
            if(mo_packet_putc(&mo, b) >0) {
                DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <=0);
            }
        }
        b = i;

        mo_packet_putc(&mo, MO_PACKET_MAGIC);
        mo_packet_putc(&mo, i);
        for(n=0; n<b; n++) {
            mo_packet_putc(&mo, b+n);
            crc = jtl_crc8_byte(crc, b+n);
        }
        mo_packet_putc(&mo, crc);

        DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) ==b);
        for(n=0; n<b; n++) {
            //DP_MSG("n=%d, b=%d", n, b);
            DP_ASSERT(pas[n] ==((uint8_t)(b+n)));
        }

        DP_ASSERT(mo_packet_parse(&mo, pas, sizeof(pas)) <0);
        DP_ASSERT(mo_buffer_get_count(&mo) ==0);
    }

    DP_MSG("mo_packet test done");
    return 0;
}

