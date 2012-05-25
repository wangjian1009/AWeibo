#include <assert.h>
#include "cpe/pal/pal_stdio.h"
#include "weibo/data/weibo_manage.h"
#include "weibo_internal_ops.h"

struct weibo_dir_ctx {
    size_t m_buf_capacity;
    size_t m_buf_base_len;
    char m_name_buf[0];
};

int weibo_dir_idx_load(weibo_manage_t mgr, void * ctx) {
    return 0;
}

int weibo_dir_idx_save(weibo_manage_t mgr, void * ctx) {
    return 0;
}

int weibo_dir_page_read(weibo_manage_t mgr, const struct weibo_page_info * page, void * ctx) {
    return 0;
}

int weibo_dir_page_write(weibo_manage_t mgr, const struct weibo_page_info * page, void * ctx) {
    return 0;
}

void weibo_dir_page_free(weibo_manage_t mgr, void * input) {
    struct weibo_dir_ctx * ctx;

    ctx = (struct weibo_dir_ctx *)input;

    mem_free(mgr->m_alloc, ctx);
}

static struct weibo_persistent g_weibo_persistent_dir = {
    weibo_dir_idx_load
    , weibo_dir_idx_save
    , weibo_dir_page_read
    , weibo_dir_page_write
    , weibo_dir_page_free
};

int weibo_manage_set_persistent_dir(weibo_manage_t mgr, const char * path, size_t max_file_len) {
    struct weibo_dir_ctx * ctx;
    size_t path_len;

    path_len = strlen(path);
    if (path_len >= max_file_len) {
        CPE_ERROR(
            mgr->m_em, "weibo: set persistent dir: dir=\"%s\", max="FMT_SIZE_T", max too small!",
            path, max_file_len);
        return -1;
    }

    ctx = mem_alloc(mgr->m_alloc, sizeof(struct weibo_dir_ctx) + max_file_len);
    if (ctx == NULL) {
        CPE_ERROR(
            mgr->m_em, "weibo: set persistent dir: dir=\"%s\", max="FMT_SIZE_T", malloc fail!",
            path, max_file_len);
        return -1;
    }

    ctx->m_buf_capacity = max_file_len;
    ctx->m_buf_base_len = path_len;
    memcpy(ctx->m_name_buf, path, path_len + 1);

    weibo_manage_set_persistent(mgr, &g_weibo_persistent_dir, ctx);
    return 0;
}
