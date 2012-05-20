#ifndef AWEIBO_DATA_TYPES_H_INCLEDED
#define AWEIBO_DATA_TYPES_H_INCLEDED
#include "cpe/pal/pal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t weibo_msg_local_it_t;

typedef struct weibo_msg_it * weibo_msg_it_t;
typedef struct weibo_msg * weibo_msg_t;
typedef struct weibo_manage * weibo_manage_t;

typedef enum {
    weibo_msg_next_fun_op_query,
    weibo_msg_next_fun_op_free
} weibo_msg_next_fun_op_t;
typedef void (*weibo_msg_next_fun_t)(weibo_msg_it_t it, weibo_msg_next_fun_op_t op, void * ctx);

#ifdef __cplusplus
}
#endif

#endif
