#ifndef WEIBO_DATA_TEST_MANAGETEST_H
#define WEIBO_DATA_TEST_MANAGETEST_H
#include "cpe/utils/tests-env/test-fixture.hpp"
#include "cpe/utils/tests-env/with_em.hpp"
#include "cpe/cfg/tests-env/with_cfg.hpp"
#include "gd/app/tests-env/with_app.hpp"
#include "weibo/data/tests-env/with_weibo_manage.hpp"
#include "../weibo_internal_ops.h"

typedef LOKI_TYPELIST_4(
    utils::testenv::with_em
    , gd::app::testenv::with_app
    , cpe::cfg::testenv::with_cfg
    , weibo::data::testenv::with_weibo_manage
    ) ManageTestBase;

class ManageTest : public testenv::fixture<ManageTestBase> {
public:
    void SetUp();
    void TearDown();
};

#endif
