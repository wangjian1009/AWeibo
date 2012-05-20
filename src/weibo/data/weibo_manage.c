#include <assert.h>
#include "cpe/pal/pal_external.h"
#include "cpe/cfg/cfg_read.h"
#include "cpe/nm/nm_manage.h"
#include "cpe/nm/nm_read.h"
#include "gd/app/app_context.h"
#include "gd/app/app_module.h"
#include "weibo/data/weibo_manage.h"
#include "weibo_internal_ops.h"

static void weibo_manage_clear(nm_node_t node);

static cpe_hash_string_buf s_weibo_manage_default_name = CPE_HS_BUF_MAKE("weibo_manage");

struct nm_node_type s_nm_node_type_weibo_manage = {
    "weibo_manage",
    weibo_manage_clear
};

weibo_manage_t
weibo_manage_create(
    gd_app_context_t app,
    const char * name,
    mem_allocrator_t alloc)
{
    weibo_manage_t mgr;
    nm_node_t mgr_node;

    if (name == 0) name = cpe_hs_data((cpe_hash_string_t)&s_weibo_manage_default_name);

    mgr_node = nm_instance_create(gd_app_nm_mgr(app), name, sizeof(struct weibo_manage));
    if (mgr_node == NULL) return NULL;

    mgr = (weibo_manage_t)nm_node_data(mgr_node);
    mgr->m_alloc = alloc;
    mgr->m_app = app;
    mgr->m_debug = 0;

    TAILQ_INIT(&mgr->m_runing_it_list);
    mgr->m_msg_id_page_free_list = NULL;

    nm_node_set_type(mgr_node, &s_nm_node_type_weibo_manage);

    return mgr;
}

static void weibo_manage_clear(nm_node_t node) {
    weibo_manage_t mgr;
    mgr = (weibo_manage_t)nm_node_data(node);

    weibo_msg_it_free_all(mgr);
    weibo_msg_id_page_list_free_all(mgr);
}

void weibo_manage_free(weibo_manage_t mgr) {
    nm_node_t mgr_node;
    assert(mgr);

    mgr_node = nm_node_from_data(mgr);
    if (nm_node_type(mgr_node) != &s_nm_node_type_weibo_manage) return;
    nm_node_free(mgr_node);
}

weibo_manage_t
weibo_manage_find(
    gd_app_context_t app,
    cpe_hash_string_t name)
{
    nm_node_t node;
    if (name == NULL) name = (cpe_hash_string_t)&s_weibo_manage_default_name;

    node = nm_mgr_find_node(gd_app_nm_mgr(app), name);
    if (node == NULL || nm_node_type(node) != &s_nm_node_type_weibo_manage) return NULL;
    return (weibo_manage_t)nm_node_data(node);
}

weibo_manage_t
weibo_manage_find_nc(
    gd_app_context_t app,
    const char * name)
{
    nm_node_t node;

    if (name == NULL) return weibo_manage_default(app);

    node = nm_mgr_find_node_nc(gd_app_nm_mgr(app), name);
    if (node == NULL || nm_node_type(node) != &s_nm_node_type_weibo_manage) return NULL;
    return (weibo_manage_t)nm_node_data(node);
}

weibo_manage_t
weibo_manage_default(
    gd_app_context_t app)
{
    return weibo_manage_find(app, (cpe_hash_string_t)&s_weibo_manage_default_name);
}

gd_app_context_t weibo_manage_app(weibo_manage_t mgr) {
    return mgr->m_app;
}

const char * weibo_manage_name(weibo_manage_t mgr) {
    return nm_node_name(nm_node_from_data(mgr));
}

cpe_hash_string_t
weibo_manage_name_hs(weibo_manage_t mgr) {
    return nm_node_name_hs(nm_node_from_data(mgr));
}

EXPORT_DIRECTIVE
int weibo_manage_app_init(gd_app_context_t app, gd_app_module_t module, cfg_t cfg) {
    weibo_manage_t weibo_manage;

    weibo_manage = weibo_manage_create(app, gd_app_module_name(module), gd_app_alloc(app));
    if (weibo_manage == NULL) return -1;

    weibo_manage->m_debug = cfg_get_int32(cfg, "debug", 0);

    if (weibo_manage->m_debug) {
        CPE_INFO(
            gd_app_em(app), "%s: create: done",
            weibo_manage_name(weibo_manage));
    }

    return 0;
}

EXPORT_DIRECTIVE
void weibo_manage_app_fini(gd_app_context_t app, gd_app_module_t module) {
    weibo_manage_t weibo_manage;

    weibo_manage = weibo_manage_find_nc(app, gd_app_module_name(module));
    if (weibo_manage) {
        weibo_manage_free(weibo_manage);
    }
}
