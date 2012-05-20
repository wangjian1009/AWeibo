#include <assert.h>
#include "weibo/data/weibo_manage.h"
#include "weibo_internal_ops.h"

inline static struct weibo_msg_id_page * weibo_msg_id_page_alloc(weibo_manage_t mgr);
inline static void weibo_msg_id_page_free(weibo_manage_t mgr, struct weibo_msg_id_page * page);

weibo_msg_it_t weibo_msg_it_create(weibo_manage_t mgr) {
    weibo_msg_it_t msg_it;

    msg_it = mem_alloc(mgr->m_alloc, sizeof(struct weibo_msg_it));
    if (msg_it == NULL) return NULL;

    msg_it->m_mgr = mgr;
    msg_it->m_id_pages_head = NULL;
    msg_it->m_id_pages_last = NULL;

    TAILQ_INSERT_TAIL(&mgr->m_runing_it_list, msg_it, m_next_it);

    return msg_it;
}

inline static void weibo_msg_it_real_free(weibo_msg_it_t msg_it) {
    weibo_manage_t mgr;

    assert(msg_it);
    assert(msg_it->m_mgr);

    mgr = msg_it->m_mgr;

    TAILQ_REMOVE(&mgr->m_runing_it_list, msg_it, m_next_it);

    mem_free(mgr->m_alloc, msg_it);
}

void weibo_msg_it_free(weibo_msg_it_t msg_it) {
    weibo_msg_it_real_free(msg_it);
}

void weibo_msg_it_free_all(weibo_manage_t mgr) {
    while(!TAILQ_EMPTY(&mgr->m_runing_it_list)) {
        weibo_msg_it_real_free(TAILQ_FIRST(&mgr->m_runing_it_list));
    }
}

weibo_msg_t weibo_msg_next(weibo_msg_it_t msg_it) {
    weibo_manage_t mgr;

    assert(msg_it);

    mgr = msg_it->m_mgr;
    assert(mgr);

    return NULL;
}

inline static
struct weibo_msg_id_page * weibo_msg_id_page_alloc(weibo_manage_t mgr) {
    struct weibo_msg_id_page * page;

    if (mgr->m_msg_id_page_free_list) {
        page = mem_alloc(mgr->m_alloc, sizeof(struct weibo_msg_id_page));
        if (page == NULL) return NULL;
    }
    else {
        page = mgr->m_msg_id_page_free_list;
        mgr->m_msg_id_page_free_list = page->m_next_page;
    }

    page->m_next_page = NULL;
    page->m_count = 0;
    page->m_cur_pos = 0;

    return page;
}

inline static
void weibo_msg_id_page_free(weibo_manage_t mgr, struct weibo_msg_id_page * page) {
    assert(mgr);
    assert(page);
    assert(page->m_next_page == NULL);

    page->m_next_page = mgr->m_msg_id_page_free_list;
    mgr->m_msg_id_page_free_list = page;
}

void weibo_msg_id_page_list_free_all(weibo_manage_t mgr) {
    assert(mgr);

    while(mgr->m_msg_id_page_free_list) {
        struct weibo_msg_id_page * page = mgr->m_msg_id_page_free_list;
        mgr->m_msg_id_page_free_list = page->m_next_page;
        mem_free(mgr->m_alloc, page);
    }
}
