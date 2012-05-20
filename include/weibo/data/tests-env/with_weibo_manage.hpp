#ifndef WEIBO_DATA_TESTENV_WITH_WEIBO_MANAGE_H
#define WEIBO_DATA_TESTENV_WITH_WEIBO_MANAGE_H
#include "cpe/utils/tests-env/test-env.hpp"
#include "../weibo_manage.h"
#include "../weibo_msg.h"

namespace weibo { namespace data { namespace testenv {

class with_weibo_manage : public ::testenv::env<> {
public:
    with_weibo_manage();

    void SetUp();
    void TearDown();

    weibo_manage_t t_weibo_manage(void);

    const char * t_weibo_msg_it_id_dump(weibo_msg_it_t msg_it);
};

}}}

#endif
