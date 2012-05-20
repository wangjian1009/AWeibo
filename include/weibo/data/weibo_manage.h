#ifndef WEIBO_DATA_MANAGE_H
#define WEIBO_DATA_MANAGE_H
#include "cpe/utils/memory.h"
#include "cpe/utils/hash_string.h"
#include "gd/app/app_types.h"
#include "weibo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

weibo_manage_t
weibo_manage_create(
    gd_app_context_t app,
    const char * name,
    mem_allocrator_t alloc);

void weibo_manage_free(weibo_manage_t mgr);

weibo_manage_t
weibo_manage_find(gd_app_context_t app, cpe_hash_string_t name);

weibo_manage_t
weibo_manage_find_nc(gd_app_context_t app, const char * name);

weibo_manage_t
weibo_manage_default(gd_app_context_t app);

gd_app_context_t weibo_manage_app(weibo_manage_t mgr);
const char * weibo_manage_name(weibo_manage_t mgr);
cpe_hash_string_t weibo_manage_name_hs(weibo_manage_t mgr);

weibo_msg_it_t weibo_msg_query_all(weibo_manage_t mgr);

#ifdef __cplusplus
}
#endif

#endif

