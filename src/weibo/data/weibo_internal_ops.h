#ifndef WEIBO_DATA_INTERNAL_OPS_H
#define WEIBO_DATA_INTERNAL_OPS_H
#include "weibo_internal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*weibo manage ops*/
int weibo_manage_read_msg(weibo_manage_t mgr, weibo_msg_t msg);
void weibo_manage_init_msg(weibo_manage_t mgr, weibo_msg_t msg);

/*weibo msg it opeationgs*/
weibo_msg_it_t weibo_msg_it_create(weibo_manage_t mgr);
void weibo_msg_it_free_all(weibo_manage_t mgr);
void weibo_msg_id_page_list_free_all(weibo_manage_t mgr);

#ifdef __cplusplus
}
#endif

#endif
