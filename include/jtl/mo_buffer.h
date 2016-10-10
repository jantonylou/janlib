#ifndef __JANLIB_MO_BUFFER_H__
#define __JANLIB_MO_BUFFER_H__

#include <stdint.h>

struct mo_buffer {
    uint8_t *buf;
    uint32_t size;
    uint32_t wrp;
    uint32_t rdp;
    int memb_size;
};

#define mo_buffer_set_memb_size(mo, size) do{(mo)->memb_size = size;}while(0)

#define mo_buffer_get_count(mo) ((mo)->wrp - (mo)->rdp)
#define mo_buffer_get_space(mo) ((mo)->size - mo_buffer_get_count(mo))
#define mo_buffer_get_size(mo)  ((mo)->size)

int mo_buffer_alloc(struct mo_buffer *mo, size_t size);
void mo_buffer_free(struct mo_buffer *mo);
void mo_buffer_clean(struct mo_buffer *mo);

int mo_buffer_write(struct mo_buffer *mo, const void *data, size_t len);
int mo_buffer_read(struct mo_buffer *mo, void *buf, size_t size);

#endif

