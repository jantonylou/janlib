#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <jtl/bits.h>
#include <jtl/mo_buffer.h>

int mo_buffer_alloc(struct mo_buffer *mo, size_t size) {
    int i;

    MibP(i, size_t, size);
    size = (1<<i)/2;

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
    int i;

    MibP(i, size_t, size);
    size = (1<<i)/2;

    memset(mo, 0, sizeof(*mo));

    mo->size = size;
    mo->buf = mem_buf;
}

void mo_buffer_clean(struct mo_buffer *mo) {
    mo->rdp = mo->wrp;
}

mo_buffer_rw_t mo_buffer_get_count(struct mo_buffer *mo)
{
    return (mo_buffer_rw_t)(mo->wrp - mo->rdp);
}

mo_buffer_rw_t mo_buffer_get_space(struct mo_buffer *mo)
{
    return (mo_buffer_rw_t)(mo->size - (mo->wrp - mo->rdp));
}

int mo_buffer_putc(struct mo_buffer *mo, uint8_t c)
{
    unsigned int wrp = mo->wrp % mo->size;

    if(mo_buffer_get_space(mo) ==0 || mo->memb_size >1) return -1;

    mo->buf[wrp] = c;
    mo->wrp++;

    return 1;
}

int mo_buffer_write(struct mo_buffer *mo, const void *data, size_t len) {
    unsigned int wrp = mo->wrp % mo->size;
    unsigned int n, space = mo_buffer_get_space(mo);

    if(space ==0) return 0;

    if(mo->memb_size >1) {
        if(space <len) return 0;
        if((len%mo->memb_size) !=0) return -1;
    } else {
        if(space <len) len = space;
    }

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

int mo_buffer_getc(struct mo_buffer *mo)
{
    unsigned int rdp = mo->rdp % mo->size;
    uint8_t c;

    if(mo_buffer_get_count(mo) ==0 || mo->memb_size >1) return -1;

    c = mo->buf[rdp];
    mo->rdp++;

    return c;
}

int mo_buffer_copy_char(struct mo_buffer *mo, unsigned int i)
{
    unsigned int rdp = (mo->rdp+i) % mo->size;

    if(mo_buffer_get_count(mo) <= i) return -1;

    return mo->buf[rdp];
}

int mo_buffer_read(struct mo_buffer *mo, void *buf, size_t size) {
    int len = mo_buffer_copy(mo, buf, size);

    if(len >0) mo->rdp += len;

    return len;
}

int mo_buffer_copy(struct mo_buffer *mo, void *buf, size_t size) {
    unsigned int rdp = mo->rdp % mo->size;
    unsigned int count = mo_buffer_get_count(mo);
    unsigned int n = mo->size - rdp;
    unsigned int len = size;

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

    return len;
}

int mo_buffer_drop(struct mo_buffer *mo, size_t len) {
    unsigned int count = mo_buffer_get_count(mo);

    if(count < len) len = count;

    if(mo->memb_size >1 && (len%mo->memb_size) !=0)
        return -1;

    mo->rdp += len;

    return len;
}

void mo_buffer_free(struct mo_buffer *mo) {
    if(mo->buf) free(mo->buf);

    memset(mo, 0, sizeof(*mo));
}

