#ifndef WEIBO_DATA_INTERNAL_TYPES_H
#define WEIBO_DATA_INTERNAL_TYPES_H
#include "cpe/pal/pal_queue.h"
#include "cpe/utils/memory.h"
#include "cpe/utils/error.h"
#include "cpe/utils/hash.h"
#include "cpe/utils/hash_string.h"
#include "gd/app/app_types.h"
#include "weibo/data/weibo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WEIBO_MSG_ID_PAGE_TOTAL_SIZE (256)
#define WEIBO_MSG_ID_PAGE_ID_CAPACITY ((WEIBO_MSG_ID_PAGE_TOTAL_SIZE \
                                        - sizeof(void*) \
                                        - sizeof(uint16_t) \
                                        - sizeof(uint16_t)) \
                                       / sizeof(weibo_msg_local_it_t))

#define WEIBO_PAGE_REFCOUNT_INVALID (0xFFFF)

typedef TAILQ_HEAD(weibo_msg_it_list, weibo_msg_it) weibo_msg_it_list_t;

struct weibo_page_info;
struct weibo_persistent;

struct weibo_manage {
    mem_allocrator_t m_alloc;
    gd_app_context_t m_app;
    error_monitor_t m_em;
    int m_debug;

    /*data page*/
    uint16_t m_pages_count;
    uint16_t m_pages_capacity;
    struct weibo_page_info * m_pages_by_id;
    struct weibo_page_info * * m_pages_by_time;

    /*msg_it*/
    weibo_msg_it_list_t m_runing_it_list;
    struct weibo_msg_id_page * m_msg_id_page_free_list;

    /*persistent*/
    struct weibo_persistent * m_persistent;
    void * m_persistent_ctx;
};

struct weibo_page_info {
    uint16_t m_page_msg_count;
    uint16_t m_ref_count;
    uint32_t m_start_time;
    uint16_t m_reserve;
    uint16_t m_data_flags;
    uint32_t m_data_capacity;
    struct weibo_msg_page_head * m_data_head;
    void * m_data_ctx;
};

struct weibo_msg {
    weibo_msg_local_it_t m_id;
};

struct weibo_msg_id_page {
    struct weibo_msg_id_page * m_next_page;
    uint16_t m_count;
    uint16_t m_cur_pos;
    weibo_msg_local_it_t m_msg_it_buf[WEIBO_MSG_ID_PAGE_ID_CAPACITY];
};

struct weibo_msg_it {
    weibo_manage_t m_mgr;
    TAILQ_ENTRY(weibo_msg_it) m_next_it;
    struct weibo_msg_id_page * m_id_pages_head;
    struct weibo_msg_id_page * m_id_pages_last;
    weibo_msg_next_fun_t m_next_fun;
    void * m_next_ctx;
    struct weibo_msg m_msg_buf;
};

typedef int (*weibo_persistent_idx_load_op_t)(weibo_manage_t mgr, void * ctx);
typedef int (*weibo_persistent_idx_save_op_t)(weibo_manage_t mgr, void * ctx);
typedef int (*weibo_persistent_page_read_op_t)(weibo_manage_t mgr, const struct weibo_page_info * page, void * ctx);
typedef int (*weibo_persistent_page_write_op_t)(weibo_manage_t mgr, const struct weibo_page_info * page, void * ctx);
typedef void (*weibo_persistent_fini_op_t)(weibo_manage_t mgr, void * ctx);

struct weibo_persistent {
    weibo_persistent_idx_load_op_t m_idx_load;
    weibo_persistent_idx_save_op_t m_idx_save;
    weibo_persistent_page_read_op_t m_page_read;
    weibo_persistent_page_write_op_t m_page_write;
    weibo_persistent_fini_op_t m_fini;
};

#ifdef __cplusplus
}
#endif

#endif
