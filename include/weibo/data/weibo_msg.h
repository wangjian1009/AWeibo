#ifndef AWEIBO_DATA_MSG_H_INCLEDED
#define AWEIBO_DATA_MSG_H_INCLEDED
#include "cpe/utils/error.h"
#include "weibo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

weibo_msgs_it_t weibo_msg_query_all(weibo_manage_t mgr);

weibo_msg_t weibo_msg_next(weibo_msgs_it_t msg_it);
void weibo_msg_it_free(weibo_msgs_it_t msg_it);

#ifdef __cplusplus
}
#endif

#endif
