#ifndef WEIBO_DATA_INTERNAL_OPS_H
#define WEIBO_DATA_INTERNAL_OPS_H
#include "weibo_internal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*weibo manage ops*/
int weibo_manage_read_msg(weibo_manage_t mgr, weibo_msg_t msg);
void weibo_manage_init_msg(weibo_manage_t mgr, weibo_msg_t msg);

void weibo_manage_set_persistent(weibo_manage_t mgr, struct weibo_persistent * persistent, void * ctx);

struct weibo_page_info * weibo_manage_page_create(
    weibo_manage_t mgr, uint16_t page_id, uint32_t start_time);

void weibo_manage_pages_clear_all(weibo_manage_t mgr);
void weibo_manage_pages_free_all(weibo_manage_t mgr);

/*weibo msg it opeationgs*/
weibo_msg_it_t weibo_msg_it_create(weibo_manage_t mgr);
void weibo_msg_it_free_all(weibo_manage_t mgr);
void weibo_msg_id_page_list_free_all(weibo_manage_t mgr);

#ifdef __cplusplus
}
#endif

#endif
