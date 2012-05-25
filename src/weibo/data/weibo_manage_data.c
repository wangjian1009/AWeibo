#include <assert.h>
#include "cpe/pal/pal_strings.h"
#include "weibo/data/weibo_manage.h"
#include "weibo_internal_ops.h"

inline static struct weibo_page_info * weibo_manage_get_page(weibo_manage_t mgr, weibo_msg_local_it_t id);
inline static void weibo_manage_page_init(struct weibo_page_info * page);

int weibo_manage_read_msg(weibo_manage_t mgr, weibo_msg_t msg) {
    uint16_t msg_pos;
    struct weibo_page_info * page;

    page = weibo_manage_get_page(mgr, msg->m_id);
    if (page == NULL) return -1;

    if (page->m_data_head == NULL) {
        if (mgr->m_persistent == NULL) return -1;

        if (mgr->m_persistent->m_page_read(mgr, page, mgr->m_persistent_ctx) != 0) {
            return -1;
        }

    }

    msg_pos = (msg->m_id & 0xFFFF);
    
    return 0;
}

void weibo_manage_init_msg(weibo_manage_t mgr, weibo_msg_t msg) {
}

inline static int weibo_manage_page_alloc_for_id(weibo_manage_t mgr, uint16_t page_id) {
    uint16_t new_page_capacity;
    struct weibo_page_info * new_page_info_buf;
    struct weibo_page_info * * new_page_by_time_buf;
    int i;

    new_page_capacity = ((page_id >> 3) + 1) << 3;
    assert(page_id < new_page_capacity);

    new_page_info_buf = mem_alloc(mgr->m_alloc, sizeof(struct weibo_page_info) * new_page_capacity);
    if (new_page_info_buf == NULL) return -1;

    new_page_by_time_buf = mem_alloc(mgr->m_alloc, sizeof(struct weibo_page_info *) * new_page_capacity);
    if (new_page_by_time_buf == NULL) {
        mem_free(mgr->m_alloc, new_page_info_buf);
        return -1;
    }

    bzero(new_page_info_buf, sizeof(struct weibo_page_info) * new_page_capacity);
    for(i = 0; i < new_page_capacity; ++i) weibo_manage_page_init(new_page_info_buf + i);

    if (mgr->m_pages_by_id) {
        memcpy(new_page_info_buf, mgr->m_pages_by_id, sizeof(struct weibo_page_info) * mgr->m_pages_capacity);
        mem_free(mgr->m_alloc, mgr->m_pages_by_id);
    }

    if (mgr->m_pages_by_time) {
        memcpy(new_page_by_time_buf, mgr->m_pages_by_time, sizeof(struct weibo_page_info) * mgr->m_pages_capacity);
        mem_free(mgr->m_alloc, mgr->m_pages_by_time);
    }

    mgr->m_pages_by_id = new_page_info_buf;
    mgr->m_pages_capacity = new_page_capacity;
    mgr->m_pages_by_time = new_page_by_time_buf;

    return 0;
}

/*page operations*/
struct weibo_page_info *
weibo_manage_page_create(weibo_manage_t mgr, uint16_t page_id, uint32_t start_time) {
    struct weibo_page_info * page_info;

    if (mgr->m_pages_capacity <= page_id) {
        if (weibo_manage_page_alloc_for_id(mgr, page_id) != 0) return NULL;
    }

    assert(page_id < mgr->m_pages_capacity);
    page_info = &mgr->m_pages_by_id[page_id];

    bzero(page_info, sizeof(*page_info));
    page_info->m_start_time = start_time;

    return page_info;
}

void weibo_manage_pages_clear_all(weibo_manage_t mgr) {
    int i;

    mgr->m_pages_count = 0;

    if (mgr->m_pages_by_id) {
        bzero(mgr->m_pages_by_id, sizeof(struct weibo_page_info) * mgr->m_pages_capacity);
        for(i = 0; i < mgr->m_pages_capacity; ++i) weibo_manage_page_init(mgr->m_pages_by_id + i);
    }
}

void weibo_manage_pages_free_all(weibo_manage_t mgr) {
    mgr->m_pages_capacity = 0;
    mgr->m_pages_count = 0;

    if (mgr->m_pages_by_id) {
        mem_free(mgr->m_alloc, mgr->m_pages_by_id);
        mgr->m_pages_by_id = NULL;
    }

    if (mgr->m_pages_by_time) {
        mem_free(mgr->m_alloc, mgr->m_pages_by_time);
        mgr->m_pages_by_time = NULL;
    }
}

inline static
struct weibo_page_info *
weibo_manage_get_page(weibo_manage_t mgr, weibo_msg_local_it_t id) {
    uint16_t page_id;
    page_id = (id & 0xFFFF0000) >> 16;

    if (page_id >= mgr->m_pages_count) {
        CPE_ERROR(
            mgr->m_em, "weibo: msg %d: page_id %d overflow, page count is %d\n",
            id, page_id, mgr->m_pages_count);
        return NULL;
    }

    return mgr->m_pages_by_id + page_id;
}

inline static void weibo_manage_page_init(struct weibo_page_info * page) {
    page->m_ref_count = WEIBO_PAGE_REFCOUNT_INVALID;
}
