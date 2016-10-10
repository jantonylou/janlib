#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <jtl/debug.h>
#include <jtl/mo_buffer.h>

int main(int argc, char *argv[]) {
    struct mo_buffer mo;
    char buf[100], buf2[100];

    DP_MSG("mo_buffer test start");

    DP_ASSERT(mo_buffer_alloc(&mo, 100) ==0);
    DP_ASSERT(mo.buf !=NULL);
    DP_ASSERT(mo.size ==100);
    DP_ASSERT(mo.wrp==0 && mo.rdp==0);

    DP_ASSERT(mo_buffer_get_count(&mo) ==0);
    DP_ASSERT(mo_buffer_get_space(&mo) ==100);

    DP_ASSERT(mo_buffer_write(&mo, "123", 4) ==4);
    DP_ASSERT(mo.wrp==4 && mo.rdp==0);
    DP_ASSERT(mo_buffer_get_count(&mo) ==4);
    DP_ASSERT(mo_buffer_get_space(&mo) ==96);

    DP_ASSERT(mo_buffer_read(&mo, buf, 1) ==1);
    DP_ASSERT(buf[0] == '1');
    DP_ASSERT(mo_buffer_get_count(&mo) ==3);
    DP_ASSERT(mo_buffer_get_space(&mo) ==97);

    DP_ASSERT(mo_buffer_read(&mo, buf, 100) ==3);
    DP_ASSERT(memcmp(buf, "23", 3) ==0);
    DP_ASSERT(mo_buffer_get_count(&mo) ==0);
    DP_ASSERT(mo_buffer_get_space(&mo) ==100);

    memset(buf, 0x55, sizeof(buf));
    DP_ASSERT(mo_buffer_write(&mo, buf, 100) ==100);
    DP_ASSERT(mo.wrp==104 && mo.rdp==4);
    DP_ASSERT(mo_buffer_get_count(&mo) ==100);
    DP_ASSERT(mo_buffer_get_space(&mo) ==0);

    DP_ASSERT(mo_buffer_read(&mo, buf2, 100) ==100);
    DP_ASSERT(memcmp(buf, buf2, 100) ==0);
    DP_ASSERT(mo.wrp==104 && mo.rdp==104);
    DP_ASSERT(mo_buffer_get_count(&mo) ==0);
    DP_ASSERT(mo_buffer_get_space(&mo) ==100);

    mo_buffer_set_memb_size(&mo, 3);
    DP_ASSERT(mo_buffer_write(&mo, "123", 4) <0);
    DP_ASSERT(mo_buffer_write(&mo, "12", 2) <0);
    DP_ASSERT(mo_buffer_write(&mo, "12", 3) ==3);
    DP_ASSERT(mo_buffer_write(&mo, "34", 3) ==3);
    DP_ASSERT(mo_buffer_read(&mo, buf, 1) <0);
    DP_ASSERT(mo_buffer_read(&mo, buf, 4) <0);
    DP_ASSERT(mo_buffer_read(&mo, buf, 3) ==3);
    DP_ASSERT(memcmp(buf, "12", 3) ==0);
    DP_ASSERT(mo_buffer_read(&mo, buf, 3) ==3);
    DP_ASSERT(memcmp(buf, "34", 3) ==0);

    mo_buffer_free(&mo);

    DP_MSG("mo_buffer test done");

    return 0;
}


