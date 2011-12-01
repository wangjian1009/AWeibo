#include "WriteTest.hpp"

TEST_F(WriteTest, data_string) {
    gd_cfg_struct_add_string(m_root, "a", "123");

    EXPECT_EQ(0, write(m_root));
    EXPECT_STREQ(
        "---\n"
        "a: !<str> 123\n"
        "...\n"
        , result());
}

TEST_F(WriteTest, map_empty) {
    EXPECT_EQ(0, write(m_root));
    EXPECT_STREQ(
        "--- {}\n"
        "...\n"
        , result());
}

TEST_F(WriteTest, map_basic) {
    gd_cfg_struct_add_int8(m_root, "a", 12);

    EXPECT_EQ(0, write(m_root));
    EXPECT_STREQ(
        "---\n"
        "a: 12\n"
        "...\n"
        , result());
}

TEST_F(WriteTest, map_map) {
    gd_cfg_t m = gd_cfg_struct_add_struct(m_root, "a");
    gd_cfg_struct_add_int8(m, "b1", 12);
    gd_cfg_struct_add_int8(m, "b2", 12);

    EXPECT_EQ(0, write(m_root));
    EXPECT_STREQ(
        "---\n"
        "a:\n"
        "    b1: 12\n"
        "    b2: 12\n"
        "...\n"
        , result());
}

TEST_F(WriteTest, seq_empty) {
    gd_cfg_t seq = gd_cfg_struct_add_seq(m_root, "a");

    EXPECT_EQ(0, write(seq));
    EXPECT_STREQ(
        "--- []\n"
        "...\n"
        , result());
}

TEST_F(WriteTest, seq_basic) {
    gd_cfg_t seq = gd_cfg_struct_add_seq(m_root, "a");

    gd_cfg_seq_add_int8(seq, 12);
    gd_cfg_seq_add_int8(seq, 13);

    EXPECT_EQ(0, write(seq));
    EXPECT_STREQ(
        "---\n"
        "- 12\n"
        "- 13\n"
        "...\n"
        , result());
}
