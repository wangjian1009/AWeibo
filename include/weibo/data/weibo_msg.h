#ifndef AWEIBO_DATA_MSG_H_INCLEDED
#define AWEIBO_DATA_MSG_H_INCLEDED
#include "cpe/utils/error.h"
#include "weibo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*msg ops*/
weibo_msg_local_it_t weibo_msg_id(weibo_msg_t msg);

/*msg it ops*/
weibo_msg_t weibo_msg_it_next(weibo_msg_it_t msg_it);
int weibo_msg_it_append(weibo_msg_it_t msg_it, weibo_msg_local_it_t local_id);
void weibo_msg_it_free(weibo_msg_it_t msg_it);
void weibo_msg_it_set_next_op(weibo_msg_it_t msg_it, weibo_msg_next_fun_t fun, void * ctx);

#ifdef __cplusplus
}
#endif

#endif
