#ifndef WEIBO_DATA_TEST_MSGITTEST_H
#define WEIBO_DATA_TEST_MSGITTEST_H
#include "ManageTest.hpp"

class MsgItTest : public ManageTest {
public:
    MsgItTest();

    virtual void SetUp();
    virtual void TearDown();

    weibo_msg_it_t msg_it_create(void);
};

#endif
