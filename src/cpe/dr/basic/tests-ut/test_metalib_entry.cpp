#include "gtest/gtest.h"
#include "cpe/dr/dr_metalib_manage.h"
#include "cpe/dr/dr_ctypes_info.h"
#include "cpe/dr/dr_error.h"
#include "with_InputMetaLibTest.hpp"

struct EntryData {
    int idx;
    int id;
    const char * name;
    const char * desc;
    const char * cname;
    int type;
    int size;
    int count;
};

class MetaLibManagerEntryTest : public ::testing::TestWithParam<EntryData>, public WithInputMetaLibTest {
public:
    MetaLibManagerEntryTest() : m_pMeta(0) {
    }

    virtual void SetUp() {
        loadLib();

        m_pMeta = dr_lib_find_meta_by_name(m_lib, "PkgHead");
        ASSERT_TRUE(m_pMeta) << "get meta PkgHead fail!";
    }

    virtual void TearDown() {
        m_pMeta = 0;
        freeLib();
    }

    LPDRMETA m_pMeta;
};

TEST_F(MetaLibManagerEntryTest, GetByIndexNetative) {
    ASSERT_EQ(NULL, dr_meta_entry_at(m_pMeta, -1));
}

TEST_F(MetaLibManagerEntryTest, GetByIndexOverflow) {
    ASSERT_EQ(NULL, dr_meta_entry_at(m_pMeta, 6));
}

TEST_F(MetaLibManagerEntryTest, GetPtrByNameExist) {
    ASSERT_EQ(NULL, dr_meta_find_entry_by_name(m_pMeta, "NotExistName"));
}

TEST_F(MetaLibManagerEntryTest, GetPtrByNameNotExist) {
    LPDRMETAENTRY pEntry = dr_meta_find_entry_by_name(m_pMeta, "version");
    ASSERT_TRUE(pEntry != NULL) << "find entry \"version\" not exist!";

    ASSERT_STREQ("version", dr_entry_name(pEntry));
}

TEST_F(MetaLibManagerEntryTest, GetByNameNotExist) {
    ASSERT_EQ(-1, dr_meta_find_entry_idx_by_name(m_pMeta, "NotExistName"));
}

TEST_F(MetaLibManagerEntryTest, GetByNameExist) {
    ASSERT_EQ(1, dr_meta_find_entry_idx_by_name(m_pMeta, "version"));
}

TEST_F(MetaLibManagerEntryTest, GetByIdNotExist) {
    ASSERT_EQ(-1, dr_meta_find_entry_idx_by_id(m_pMeta, 99));
}

TEST_F(MetaLibManagerEntryTest, GetByIdExist) {
    ASSERT_EQ(1, dr_meta_find_entry_idx_by_id(m_pMeta, 12));
}

TEST_F(MetaLibManagerEntryTest, GetByPathNoNestedExist) {
    LPDRMETAENTRY pstEntry =
        dr_meta_find_entry_by_path(
            dr_lib_find_meta_by_name(m_lib, "Pkg"),
            "head");

    ASSERT_TRUE(pstEntry) << "head of Pkg not exist";

    ASSERT_STREQ("head", dr_entry_name(pstEntry));
}

TEST_F(MetaLibManagerEntryTest, GetByPathNoNestedNotExist) {
    LPDRMETAENTRY pstEntry =
        dr_meta_find_entry_by_path(
            dr_lib_find_meta_by_name(m_lib, "Pkg"),
            "not-exist");

    ASSERT_TRUE(pstEntry == 0) << "not-exist entry of Pkg should find fail!";
}

TEST_F(MetaLibManagerEntryTest, GetByPathNested) {
    LPDRMETAENTRY pstEntry =
        dr_meta_find_entry_by_path(
            dr_lib_find_meta_by_name(m_lib, "Pkg"),
            "head.bodylen");

    ASSERT_TRUE(pstEntry != NULL) << "head.bodylen of Pkg not exist";

    ASSERT_STREQ("bodylen", dr_entry_name(pstEntry));
}

TEST_F(MetaLibManagerEntryTest, GetByPathNestedLastNotExist) {
    LPDRMETAENTRY pstEntry =
        dr_meta_find_entry_by_path(
            dr_lib_find_meta_by_name(m_lib, "Pkg"),
            "head.not-exist");

    ASSERT_TRUE(pstEntry == NULL) << "head.not-exist of Pkg not exist";
}

TEST_F(MetaLibManagerEntryTest, GetByPathNestedMiddleNotExist) {
    LPDRMETAENTRY pstEntry =
        dr_meta_find_entry_by_path(
            dr_lib_find_meta_by_name(m_lib, "Pkg"),
            "not-exist.bodylen");

    ASSERT_TRUE(pstEntry == NULL) << "not-exist.bodylen of Pkg not exist";
}

TEST_P(MetaLibManagerEntryTest, CheckData) {
    EntryData entry = GetParam();

    LPDRMETAENTRY pEntry = dr_meta_entry_at(m_pMeta, entry.idx);
    ASSERT_TRUE(pEntry) << "get entry at " << entry.idx << " fail!";

    ASSERT_STREQ(entry.name, dr_entry_name(pEntry));

    ASSERT_STREQ(entry.desc, dr_entry_desc(pEntry));

    ASSERT_STREQ(entry.cname, dr_entry_cname(pEntry));

    ASSERT_EQ(entry.id, dr_entry_id(pEntry));

    ASSERT_EQ(entry.type, dr_entry_type(pEntry));

    ASSERT_EQ(entry.size, dr_entry_size(pEntry));

    ASSERT_EQ(entry.count, dr_entry_array_count(pEntry));

    ASSERT_TRUE(dr_entry_self_meta(pEntry) == m_pMeta) << "get meta from entry error!";
}

EntryData entryDataCases[] = {
    /* idx id  name       desc           cname            type                  size count*/
    {  0,  11, "magic",   "",            "",              CPE_DR_TYPE_INT16, 2,   1}
    , {1,  12, "version", "",            "",              CPE_DR_TYPE_INT16, 2,   1}
    , {2,  -1, "time",    "",            "",              CPE_DR_TYPE_DATETIME, 8,   1}
    , {3,  -1, "cmd",     "",            "",              CPE_DR_TYPE_INT16, 2,   1}
    , {4,  -1, "bodylen", "",            "bodylen.cname", CPE_DR_TYPE_INT16, 2,   1}
    , {5,  -1, "msgid",   "message id",  "",              CPE_DR_TYPE_UINT32,     4,   1}
};

INSTANTIATE_TEST_CASE_P(
    CheckAllEntryData,
    MetaLibManagerEntryTest,
    testing::ValuesIn(entryDataCases));
