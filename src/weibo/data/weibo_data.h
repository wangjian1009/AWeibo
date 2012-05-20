#ifndef WEIBO_DATA_PAGE_H
#define WEIBO_DATA_PAGE_H
#include "cpe/pal/pal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct weibo_msg_basic {
    uint16_t m_creation_time;
    uint16_t m_creation_user;
    uint32_t m_data_pos;
};

struct weibo_msg_page_head {
    uint32_t m_contain_begin_time;
    uint32_t m_contain_end_time;
    int16_t m_msg_capacity;
    int16_t m_msg_count;
};

struct weibo_user_basic {
    uint16_t m_user_local_id;
};

struct weibo_user_page_head {
    
};
 
#ifdef __cplusplus
}
#endif

#endif
