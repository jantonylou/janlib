#ifndef __JANLIB_MO_BUFFER_H__
#define __JANLIB_MO_BUFFER_H__

#include <stdint.h>
#include <sys/types.h>

#ifdef JTL_MO_BUFFER_16BIT
typedef uint16_t mo_buffer_rw_t;
#else
typedef uint32_t mo_buffer_rw_t;
#endif

struct mo_buffer {
    uint8_t *buf;   // main buffer

    mo_buffer_rw_t size;    // MUST BE 2^n
    mo_buffer_rw_t wrp;
    mo_buffer_rw_t rdp;
    mo_buffer_rw_t memb_size;

#ifdef JTL_MO_BUFFER_PRIV_DATA
    uint8_t priv_data[JTL_MO_BUFFER_PRIV_DATA]; // Private data used by 3rd party
#endif
};

#define mo_buffer_set_memb_size(mo, size) do{(mo)->memb_size = size;}while(0)

mo_buffer_rw_t mo_buffer_get_count(struct mo_buffer *mo);
mo_buffer_rw_t mo_buffer_get_space(struct mo_buffer *mo);
#define mo_buffer_get_size(mo)  ((mo)->size)

int mo_buffer_alloc(struct mo_buffer *mo, size_t size);
void mo_buffer_free(struct mo_buffer *mo);

// size MUST BE 2^n
void mo_buffer_init(struct mo_buffer *mo, void *mem_buf, size_t size);

void mo_buffer_clean(struct mo_buffer *mo);

// return 1 for success, -1 for error
int mo_buffer_putc(struct mo_buffer *mo, uint8_t c);
// return writed bytes, -1 for error
int mo_buffer_write(struct mo_buffer *mo, const void *data, size_t len);
// return bytes read out
int mo_buffer_read(struct mo_buffer *mo, void *buf, size_t size);

// read out a char, -1 for error
int mo_buffer_getc(struct mo_buffer *mo);

int mo_buffer_copy_char(struct mo_buffer *mo, unsigned int i);
#define mo_buffer_copy_first(mo) mo_buffer_copy_char(mo, 0)

// return bytes read out
int mo_buffer_copy(struct mo_buffer *mo, void *buf, size_t size);

// return bytes dropped
int mo_buffer_drop(struct mo_buffer *mo, size_t len);
#define mo_buffer_drop_char(mo) mo_buffer_drop(mo, 1)

#endif

