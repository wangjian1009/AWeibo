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
    msg_it->m_next_fun = NULL;
    msg_it->m_next_ctx = NULL;

    TAILQ_INSERT_TAIL(&mgr->m_runing_it_list, msg_it, m_next_it);

    return msg_it;
}

inline static void weibo_msg_it_real_free(weibo_msg_it_t msg_it) {
    weibo_manage_t mgr;

    assert(msg_it);
    assert(msg_it->m_mgr);

    mgr = msg_it->m_mgr;

    if (msg_it->m_next_fun) {
        msg_it->m_next_fun(msg_it, weibo_msg_next_fun_op_free, msg_it->m_next_ctx);
    }

    while(msg_it->m_id_pages_head) {
        struct weibo_msg_id_page * page;

        page = msg_it->m_id_pages_head;
        msg_it->m_id_pages_head = page->m_next_page;
        page->m_next_page = NULL;

        weibo_msg_id_page_free(mgr, page);
    }

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

inline static void weibo_msg_it_remove_first_page(weibo_msg_it_t msg_it) {
    struct weibo_msg_id_page * page;

    page = msg_it->m_id_pages_head;
    assert(page);

    msg_it->m_id_pages_head = page->m_next_page;

    if (page == msg_it->m_id_pages_last) {
        assert(msg_it->m_id_pages_head == NULL);
        msg_it->m_id_pages_last = NULL;
    }

    page->m_next_page = NULL;
    weibo_msg_id_page_free(msg_it->m_mgr, page);
}

weibo_msg_t weibo_msg_it_next(weibo_msg_it_t msg_it) {
    weibo_manage_t mgr;

    assert(msg_it);

    mgr = msg_it->m_mgr;
    assert(mgr);

    if (msg_it->m_id_pages_head == NULL) {
        if (msg_it->m_next_fun == NULL) return NULL;

        /*load next page*/
        msg_it->m_next_fun(msg_it, weibo_msg_next_fun_op_query, msg_it->m_next_ctx);

        if (msg_it->m_id_pages_head == NULL) { /*load next page fail*/
            msg_it->m_next_fun(msg_it, weibo_msg_next_fun_op_free, msg_it->m_next_ctx);
            msg_it->m_next_fun = NULL;
            msg_it->m_next_ctx = NULL;
            return NULL;
        }
    }

    while(msg_it->m_id_pages_head && msg_it->m_id_pages_head->m_cur_pos >= msg_it->m_id_pages_head->m_count) {
        weibo_msg_it_remove_first_page(msg_it);
    }

    if (msg_it->m_id_pages_head == NULL) return NULL;

    assert(msg_it->m_id_pages_head->m_cur_pos < msg_it->m_id_pages_head->m_count);

    msg_it->m_msg_buf.m_id =
        msg_it->m_id_pages_head->m_msg_it_buf[msg_it->m_id_pages_head->m_cur_pos++];
    while(msg_it->m_id_pages_head && msg_it->m_id_pages_head->m_cur_pos >= msg_it->m_id_pages_head->m_count) {
        weibo_msg_it_remove_first_page(msg_it);
    }

    if (weibo_manage_read_msg(mgr, &msg_it->m_msg_buf) != 0) {
        weibo_manage_init_msg(mgr, &msg_it->m_msg_buf);
    }

    return &msg_it->m_msg_buf;
}

inline static
struct weibo_msg_id_page * weibo_msg_id_page_alloc(weibo_manage_t mgr) {
    struct weibo_msg_id_page * page;

    if (mgr->m_msg_id_page_free_list == NULL) {
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

int weibo_msg_it_append(weibo_msg_it_t msg_it, weibo_msg_local_it_t local_id) {
    struct weibo_msg_id_page * page;

    page = msg_it->m_id_pages_last;

    if (page == NULL || page->m_count >= WEIBO_MSG_ID_PAGE_ID_CAPACITY) {
        page = weibo_msg_id_page_alloc(msg_it->m_mgr);
        if (page == NULL) return -1;

        assert(page->m_count == 0);

        if (msg_it->m_id_pages_last == NULL) {
            msg_it->m_id_pages_last = page;
            assert(msg_it->m_id_pages_head == NULL);
            msg_it->m_id_pages_head = page;
        }
        else {
            assert(msg_it->m_id_pages_last->m_next_page == NULL);
            page->m_next_page = msg_it->m_id_pages_last;
            msg_it->m_id_pages_last = page;
        }
    }

    assert(page);
    assert(page->m_count < WEIBO_MSG_ID_PAGE_ID_CAPACITY);

    page->m_msg_it_buf[page->m_count] = local_id;
    ++page->m_count;

    return 0;
}

void weibo_msg_it_set_next_op(weibo_msg_it_t msg_it, weibo_msg_next_fun_t fun, void * ctx) {
    if (msg_it->m_next_fun) {
        msg_it->m_next_fun(msg_it, weibo_msg_next_fun_op_free, msg_it->m_next_ctx);
    }
    msg_it->m_next_fun = fun;
    msg_it->m_next_ctx = ctx;
}
