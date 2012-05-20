#include "cpe/utils/buffer.h"
#include "cpe/utils/stream_buffer.h"
#include "gd/app/tests-env/with_app.hpp"
#include "weibo/data/tests-env/with_weibo_manage.hpp"

namespace weibo { namespace data { namespace testenv {

with_weibo_manage::with_weibo_manage() {
}

void with_weibo_manage::SetUp() {
    ASSERT_TRUE(
        weibo_manage_create(
            envOf<gd::app::testenv::with_app>().t_app(),
            NULL,
            t_allocrator()));
}

void with_weibo_manage::TearDown() {
    weibo_manage_t mgr = 
        weibo_manage_default(envOf<gd::app::testenv::with_app>().t_app());
    if (mgr) weibo_manage_free(mgr);
}

weibo_manage_t
with_weibo_manage::t_weibo_manage(void) {
    return weibo_manage_default(envOf<gd::app::testenv::with_app>().t_app());
}

const char * with_weibo_manage::t_weibo_msg_it_id_dump(weibo_msg_it_t msg_it) {
    struct mem_buffer buffer;
    mem_buffer_init(&buffer, 0);

    write_stream_buffer stream = CPE_WRITE_STREAM_BUFFER_INITIALIZER(&buffer);

    int first = 1;
    weibo_msg_t msg;
    while((msg = weibo_msg_it_next(msg_it))) {
        if (first) {
            first = 0;
        }
        else {
            stream_printf((write_stream_t)&stream, " ");
        }

        stream_printf((write_stream_t)&stream, "%u", weibo_msg_id(msg));
    }
    stream_putc((write_stream_t)&stream, 0);

    const char * r =  t_tmp_strdup((const char *)mem_buffer_make_continuous(&buffer, 0));
    mem_buffer_clear(&buffer);
    return r;
}

}}}
