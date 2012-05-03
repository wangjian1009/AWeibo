#include "cpe/dr/dr_metalib_manage.h"
#include "WriteTest.hpp"

TEST_F(WriteTest, type_uin32) {
    installMeta(
        "<metalib tagsetversion='1' name='net'  version='1'>"
        "    <struct name='S' version='1'>"
        "	     <entry name='a1' type='uint32' id='1'/>"
        "    </struct>"
        "</metalib>"
        );

    EXPECT_EQ(3, write("S", "a1: 150"));

    EXPECT_STREQ(
        "0x08 0x96 0x01", result());
}

TEST_F(WriteTest, type_string) {
    installMeta(
        "<metalib tagsetversion='1' name='net'  version='1'>"
        "    <struct name='S' version='1'>"
        "	     <entry name='a1' type='string' id='2' size='128'/>"
        "    </struct>"
        "</metalib>"
        );

    EXPECT_EQ(9, write("S", "a1: testing"));
    
    EXPECT_STREQ(
        "0x12 0x07 0x74 0x65 0x73 0x74 0x69 0x6E 0x67", result());
}
