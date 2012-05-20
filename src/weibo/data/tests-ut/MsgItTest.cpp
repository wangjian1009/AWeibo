#include "MsgItTest.hpp"

MsgItTest::MsgItTest()
{
}

void MsgItTest::SetUp() {
    ManageTest::SetUp();
}

void MsgItTest::TearDown() {
    ManageTest::TearDown();
}

weibo_msg_it_t MsgItTest::msg_it_create(void) {
    return weibo_msg_it_create(t_weibo_manage());
}
