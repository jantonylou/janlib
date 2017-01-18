#ifndef __JANLIB_BIT_STREAM_H__
#define __JANLIB_BIT_STREAM_H__

#include <stdint.h>

inline static void bits_setx(uint8_t *bs, int nbits, int x) {
    int i=nbits/8;
    int n=nbits%8;

    if(x) x=1;

    bs[i] &= ~(1<<n);
    bs[i] |= x<<n;
};
#define bits_set(bs, n) bits_setx(bs, n, 1)
#define bits_clr(bs, n) bits_setx(bs, n, 0)

inline static int bits_test(uint8_t *bs, int nbits, int x) {
    int i=nbits/8;
    int n=nbits%8;

    if(x) x=1;
    return (((int)bs[i] >>n)&0x01)==x ? 1 : 0;
}
#define bits_isset(bs, n) bits_test(bs, n, 1)
#define bits_isclr(bs, n) bits_test(bs, n, 0)

inline static int bits_find(uint8_t *bs, int nlimit, int x) {
    uint8_t M = x==0 ? 0xFF : 0, b;
    int i, j, N=nlimit/8, K=nlimit%8;

    for(i=0; i<N && bs[i]==M; i++){};
    b = bs[i];
    if(i<N) K=8;

    for(j=0; j<K; j++){
        if(bits_test(&b, j, x)) return i*8 + j;
    };

    return -1;
};

// Most important bit position, when x=0, n=0
#define MibP(n, type, x) do{ type _MibP_x=x; \
    for(n=0; (_MibP_x)!=0; n++) { _MibP_x >>=1; } \
}while(0)

#endif
