#include <assert.h>
#include "cpe/cfg/cfg_manage.h"
#include "cpe/pal/pal_strings.h"
#include "cpe/pal/pal_unistd.h"
#include "gd/tl/tl_manage.h"
#include "gd/app/app_context.h"
#include "app_internal_ops.h"

static int gd_app_chdir_to_root(gd_app_context_t context) {
    if (context->m_root) {
        if (chdir(context->m_root) != 0) {
            CPE_ERROR(gd_app_em(context), "change root to %s fail!", context->m_root);
            return -1;
        }
    }

    return 0;
}

int gd_app_run(gd_app_context_t context) {
    int rv;

    assert(context);

    if (g_app_context != NULL) {
        CPE_ERROR(context->m_em, "app already runing!");
        return -1;
    }

    g_app_context = context;

    if (gd_app_chdir_to_root(context) != 0) {
        g_app_context = NULL;
        return -1;
    }

    if (gd_app_cfg_reload(context) != 0) {
        g_app_context = NULL;
        return -1;
    }

    if (gd_app_tick_add(context, (gd_app_tick_fun)gd_tl_manage_tick, context->m_tl_mgr, 500) != 0) {
        g_app_context = NULL;
        return -1;
    }

    if (gd_app_modules_load(context) != 0) {
        g_app_context = NULL;
        return -1;
    }

    if (context->m_main == NULL) {
        CPE_ERROR(context->m_em, "no main function to runing!");
        gd_app_modules_unload(context);
        g_app_context = NULL;
        return -1;
    }

    rv = context->m_main(context);

    gd_app_modules_unload(context);
    g_app_context = NULL;

    return rv;
}
