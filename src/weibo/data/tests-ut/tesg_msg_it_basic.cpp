#include "MsgItTest.hpp"

TEST_F(MsgItTest, empty) {
    weibo_msg_it_t msg_it = msg_it_create();
    ASSERT_TRUE(msg_it);

    EXPECT_TRUE(weibo_msg_it_next(msg_it) == NULL);
}

TEST_F(MsgItTest, add_to_empty) {
    weibo_msg_it_t msg_it = msg_it_create();
    ASSERT_TRUE(msg_it);

    weibo_msg_it_append(msg_it, 0);

    EXPECT_STREQ(
        "0",
        t_weibo_msg_it_id_dump(msg_it));
}
