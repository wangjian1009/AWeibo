#include "cpe/utils/stream_mem.h"
#include "cpe/utils/error.h"
#include "cpe/cfg/cfg_manage.h"
#include "gd/app/tests-env/with_app.hpp"
#include "cpe/utils/tests-env/with_em.hpp"

namespace gd { namespace app { namespace testenv {

with_app::with_app()
    : m_app(NULL)
{
}

void with_app::SetUp() {
    Base::SetUp();
}

void with_app::TearDown() {
    gd_app_context_free(m_app);
    m_app = NULL;
    Base::TearDown();
}

gd_app_context_t
with_app::t_app() {
    if (m_app == NULL) {
        t_app_create(0, 0, NULL);
    }

    return m_app;
}

void with_app::t_app_create(size_t capacity, int argc, char ** argv) {
    if (m_app) {
        gd_app_context_free(m_app);
        m_app = NULL;
    }

    m_app =
        gd_app_context_create(t_allocrator(), capacity, argc, argv);

    ASSERT_TRUE(m_app) << "create app fail!" ;


    if (utils::testenv::with_em * env_em = tryEnvOf<utils::testenv::with_em>()) {
        gd_app_set_em(m_app, env_em->t_em());
    }
}

gd_app_module_t
with_app::t_app_install_module(
    const char * name,
    const char * libName,
    const char * strCfg)
{
    cfg_t cfg = NULL;
    if (strCfg) {
        cfg = cfg_create(t_tmp_allocrator());
        EXPECT_TRUE(cfg) << "create cfg for install module fail!";

        struct read_stream_mem stream =
            CPE_READ_STREAM_MEM_INITIALIZER(strCfg, strlen(strCfg));

        EXPECT_EQ(0, cfg_read(cfg, (read_stream_t)&stream, cfg_replace, NULL));
    }

    gd_app_module_t m =gd_app_install_module(t_app(), name, libName, cfg);

    if (cfg) {
        cfg_free(cfg);
    }

    return m;
}

gd_app_module_t
with_app::t_app_install_module(
    const char * name,
    const char * cfg)
{
    return t_app_install_module(name, NULL, cfg);
}

}}}
