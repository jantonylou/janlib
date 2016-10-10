#ifndef __JANLIB_DEBUG_H__
#define __JANLIB_DEBUG_H__

#define DP_ASSERT(cond) do{ if(!(cond)) {   \
    fprintf(stderr, "\033[33mASSERT_FAILED: %s:%d: %s\r\n\033[0m", __FILE__, __LINE__, #cond);  \
} }while(0)

#define DP_MSG(s...) do{ \
    printf("DP_MSG: %s:%d: ", __FILE__, __LINE__); \
    printf(s); \
    printf("\r\n"); \
}while(0)

#define DP_WRN(s...) do{ \
    fprintf(stderr, "\033[31mDP_WRN: %s:%d: ", __FILE__, __LINE__); \
    fprintf(stderr, s); \
    fprintf(stderr, "\r\n\033[0m"); \
}while(0)

#define DP_ERR(s...) do{ \
    fprintf(stderr, "\033[41mDP_ERR: %s:%d: ", __FILE__, __LINE__); \
    fprintf(stderr, s); \
    fprintf(stderr, "\r\n\033[0m"); \
}while(0)

#endif

