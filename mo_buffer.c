#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <jtl/mo_buffer.h>

int mo_buffer_alloc(struct mo_buffer *mo, size_t size) {
    memset(mo, 0, sizeof(*mo));

    mo->buf = malloc(size); 
    if(mo->buf ==NULL)
        return -1;

    mo->size = size;
    mo->wrp = 0;
    mo->rdp = 0;

    return 0;
}

void mo_buffer_init(struct mo_buffer *mo, void *mem_buf, size_t size) {
    memset(mo, 0, sizeof(*mo));
    mo->buf = mem_buf;
    mo->size = size;
}

void mo_buffer_clean(struct mo_buffer *mo) {
    mo->rdp = mo->wrp;
}

int mo_buffer_write(struct mo_buffer *mo, const void *data, size_t len) {
    uint32_t wrp = mo->wrp % mo->size;
    size_t n;

    if(mo->memb_size >1 && (len%mo->memb_size) !=0)
        return -1;

    if(mo_buffer_get_space(mo) <len)
        return -1;

    n = mo->size - wrp;

    if(len <= n) {
        memcpy(mo->buf+wrp, data, len);
    } else {
        memcpy(mo->buf+wrp, data, n);
        memcpy(mo->buf, ((const uint8_t*)data) +n, len-n);
    }

    mo->wrp += len;

    return len;
}

int mo_buffer_read(struct mo_buffer *mo, void *buf, size_t size) {
    uint32_t rdp = mo->rdp % mo->size;
    uint32_t count = mo_buffer_get_count(mo);
    uint32_t n = mo->size - rdp;
    size_t len = size;

    if(n >count) n = count;
    if(len >count) len = count;

    if(mo->memb_size >1 && (len%mo->memb_size) !=0)
        return -1;

    if(len <= n) {
        memcpy(buf, mo->buf+rdp, len);
    } else {
        memcpy(buf, mo->buf+rdp, n);
        memcpy(((uint8_t*)buf)+n, mo->buf, len-n);
    }

    mo->rdp += len;

    return len;
}

void mo_buffer_free(struct mo_buffer *mo) {
    if(mo->buf) free(mo->buf);

    memset(mo, 0, sizeof(*mo));
}

