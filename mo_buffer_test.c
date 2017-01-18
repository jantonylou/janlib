#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <jtl/debug.h>
#include <jtl/mo_buffer.h>

int main(int argc, char *argv[]) {
    int i;
    mo_buffer_rw_t set_wrp;
    struct mo_buffer mo;
    char buf[128], buf2[128];

    DP_MSG("mo_buffer test start");

    mo_buffer_init(&mo, buf, 100);
    DP_ASSERT(mo.size ==64);
    mo_buffer_init(&mo, buf, sizeof(buf));
    DP_ASSERT(mo.size ==sizeof(buf));

    DP_ASSERT(mo_buffer_alloc(&mo, 100) ==0);
    DP_ASSERT(mo.buf !=NULL);
    DP_ASSERT(mo.size ==64);
    DP_ASSERT(mo.wrp==0 && mo.rdp==0);

    DP_ASSERT(mo_buffer_get_count(&mo) ==0);
    DP_ASSERT(mo_buffer_get_space(&mo) ==64);

    DP_ASSERT(mo_buffer_write(&mo, "123", 4) ==4);
    DP_ASSERT(mo.wrp==4 && mo.rdp==0);
    DP_ASSERT(mo_buffer_get_count(&mo) ==4);
    DP_ASSERT(mo_buffer_get_space(&mo) ==60);

    DP_ASSERT(mo_buffer_read(&mo, buf, 1) ==1);
    DP_ASSERT(buf[0] == '1');
    DP_ASSERT(mo_buffer_get_count(&mo) ==3);
    DP_ASSERT(mo_buffer_get_space(&mo) ==61);

    DP_ASSERT(mo_buffer_read(&mo, buf, 100) ==3);
    DP_ASSERT(memcmp(buf, "23", 3) ==0);
    DP_ASSERT(mo_buffer_get_count(&mo) ==0);
    DP_ASSERT(mo_buffer_get_space(&mo) ==64);

    memset(buf, 0x55, sizeof(buf));
    DP_ASSERT(mo_buffer_write(&mo, buf, 100) ==64);
    DP_ASSERT(mo.wrp==68 && mo.rdp==4);
    DP_ASSERT(mo_buffer_get_count(&mo) ==64);
    DP_ASSERT(mo_buffer_get_space(&mo) ==0);

    DP_ASSERT(mo_buffer_read(&mo, buf2, 100) ==64);
    DP_ASSERT(memcmp(buf, buf2, 64) ==0);
    DP_ASSERT(mo.wrp==68 && mo.rdp==68);
    DP_ASSERT(mo_buffer_get_count(&mo) ==0);
    DP_ASSERT(mo_buffer_get_space(&mo) ==64);

    mo_buffer_set_memb_size(&mo, 3);
    DP_ASSERT(mo_buffer_write(&mo, "123", 4) <0);
    DP_ASSERT(mo_buffer_getc(&mo) <0);
    DP_ASSERT(mo_buffer_copy_char(&mo, 0) <0);
    DP_ASSERT(mo_buffer_write(&mo, "12", 2) <0);
    DP_ASSERT(mo_buffer_write(&mo, "12", 3) ==3);
    DP_ASSERT(mo_buffer_write(&mo, "34", 3) ==3);
    DP_ASSERT(mo_buffer_read(&mo, buf, 1) <0);
    DP_ASSERT(mo_buffer_read(&mo, buf, 4) <0);
    DP_ASSERT(mo_buffer_read(&mo, buf, 3) ==3);
    DP_ASSERT(memcmp(buf, "12", 3) ==0);
    DP_ASSERT(mo_buffer_read(&mo, buf, 3) ==3);
    DP_ASSERT(memcmp(buf, "34", 3) ==0);

    DP_ASSERT(mo_buffer_putc(&mo, 'X') <0);

    mo_buffer_set_memb_size(&mo, 0);
    DP_ASSERT(mo_buffer_putc(&mo, 'X') ==1);
    DP_ASSERT(mo_buffer_getc(&mo) =='X');

    DP_ASSERT(mo_buffer_write(&mo, "ABC", 3) ==3);
    DP_ASSERT(mo_buffer_copy_char(&mo, 0) =='A');
    DP_ASSERT(mo_buffer_copy_char(&mo, 1) =='B');
    DP_ASSERT(mo_buffer_copy_char(&mo, 2) =='C');
    DP_ASSERT(mo_buffer_copy_char(&mo, 3) <0);
    DP_ASSERT(mo_buffer_drop(&mo, 3) ==3);

    mo.wrp = mo.rdp = 0;
    for(i=0; i<10000000; i++) {
        int w;
        DP_ASSERT(mo_buffer_write(&mo, &i, sizeof(i)) ==sizeof(i));
        DP_ASSERT(mo.wrp ==(mo_buffer_rw_t)((i+1)*sizeof(i)));
        DP_ASSERT(mo_buffer_read(&mo, &w, sizeof(w)) ==sizeof(w));
        DP_ASSERT(mo.rdp ==(mo_buffer_rw_t)((i+1)*sizeof(w)));
        DP_ASSERT(w ==i);
    }

    mo.wrp = mo.rdp = 1;
    for(i=0; i<10000000; i++) {
        int w;
        DP_ASSERT(mo_buffer_write(&mo, &i, sizeof(i)) ==sizeof(i));
        DP_ASSERT(mo.wrp ==(mo_buffer_rw_t)((i+1)*sizeof(i)+1));
        DP_ASSERT(mo_buffer_read(&mo, &w, sizeof(w)) ==sizeof(w));
        DP_ASSERT(mo.rdp ==(mo_buffer_rw_t)((i+1)*sizeof(w)+1));
        DP_ASSERT(w ==i);
    }

    mo.wrp = mo.rdp = 2;
    for(i=0; i<10000000; i++) {
        int w;
        DP_ASSERT(mo_buffer_write(&mo, &i, sizeof(i)) ==sizeof(i));
        DP_ASSERT(mo.wrp ==(mo_buffer_rw_t)((i+1)*sizeof(i)+2));
        DP_ASSERT(mo_buffer_read(&mo, &w, sizeof(w)) ==sizeof(w));
        DP_ASSERT(mo.rdp ==(mo_buffer_rw_t)((i+1)*sizeof(w)+2));
        DP_ASSERT(w ==i);
    }

    mo.wrp = mo.rdp = 3;
    for(i=0; i<10000000; i++) {
        int w;
        DP_ASSERT(mo_buffer_write(&mo, &i, sizeof(i)) ==sizeof(i));
        DP_ASSERT(mo.wrp ==(mo_buffer_rw_t)((i+1)*sizeof(i)+3));
        DP_ASSERT(mo_buffer_read(&mo, &w, sizeof(w)) ==sizeof(w));
        DP_ASSERT(mo.rdp ==(mo_buffer_rw_t)((i+1)*sizeof(w)+3));
        DP_ASSERT(w ==i);
    }

    mo.wrp = 1<<(sizeof(mo_buffer_rw_t)*8 -1);
    mo.wrp += mo.wrp - 10000;
    mo.rdp = mo.wrp;
    set_wrp = mo.wrp;

    DP_MSG("mo.wrp=0x%X, mo.rdp=0x%X", mo.wrp, mo.rdp);

    for(i=0; i<10000000; i++) {
        int w;
        DP_ASSERT(mo_buffer_write(&mo, &i, sizeof(i)) ==sizeof(i));
        DP_ASSERT(mo.wrp ==(mo_buffer_rw_t)((i+1)*sizeof(i)+set_wrp));
        DP_ASSERT(mo_buffer_read(&mo, &w, sizeof(w)) ==sizeof(w));
        DP_ASSERT(mo.rdp ==(mo_buffer_rw_t)((i+1)*sizeof(w)+set_wrp));
        DP_ASSERT(w ==i);
    }

    DP_MSG("mo.wrp=0x%X, mo.rdp=0x%X", mo.wrp, mo.rdp);

    mo_buffer_free(&mo);

    DP_MSG("mo_buffer test done");

    return 0;
}


