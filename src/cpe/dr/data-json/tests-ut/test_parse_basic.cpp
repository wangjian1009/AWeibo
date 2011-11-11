#include "cpe/dr/dr_metalib_manage.h"
#include "ParseTest.hpp"

TEST_F(ParseTest, metalib_basic) {
    installMeta(
        "<metalib tagsetversion='1' name='net'  version='1'>"
        "    <struct name='S' version='1'>"
        "	     <entry name='a1' type='int16'/>"
        "    </struct>"
        "</metalib>"
        );

    struct {
        int16_t a1;
    } expect = { 12 };

    ASSERT_EQ(0, read("{ \"a1\" : 12}", "S"));

    ASSERT_JSON_READ_RESULT(expect);
}

TEST_F(ParseTest, metalib_nest) {
    installMeta(
        "<metalib tagsetversion='1' name='net'  version='1'>"
        "    <struct name='S' version='1'>"
        "	     <entry name='a1' type='int16'/>"
        "    </struct>"
        "    <struct name='S2' version='1'>"
        "	     <entry name='m_s' type='S'/>"
        "	     <entry name='a2' type='int16'/>"
        "    </struct>"
        "</metalib>"
        );

#pragma pack(push,1)
    struct {
        struct {
            int16_t a1;
        } m_s;
        int16_t a2;
    } expect = { { 12 }, 14  };
#pragma pack(pop)

    ASSERT_EQ(0, read("{ \"m_s\" : { \"a1\" : 12 }, \"a2\" : 14 }", "S2"));

    //memcpy(&expect, result(), sizeof(expect));
    //printf("m_s.a1=%d, a2=%d\n", expect.m_s.a1, expect.a2);

    ASSERT_JSON_READ_RESULT(expect);
}
