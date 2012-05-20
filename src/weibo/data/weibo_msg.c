#include <assert.h>
#include "weibo/data/weibo_msg.h"
#include "weibo_internal_ops.h"

weibo_msg_local_it_t weibo_msg_id(weibo_msg_t msg) {
    return msg->m_id;
}
