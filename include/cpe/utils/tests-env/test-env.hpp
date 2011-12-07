#ifndef CPE_UTILS_TEST_EXTERN_H
#define CPE_UTILS_TEST_EXTERN_H
#include "test-fixture.hpp"

namespace testenv {

template <typename TList> struct env_gen;

template <class Head, class Tail>
struct env_gen<Loki::Typelist<Head, Tail> >
    : public Head
    , public env_gen<Tail>
{
    void SetUp() {
        Head::SetUp();
        env_gen<Tail>::SetUp();
    }

    void TearDown() {
        env_gen<Tail>::TearDown();
        Head::TearDown();
    }
};

template <>
struct env_gen<Loki::NullType> {
    void SetUp() {}
    void TearDown() {}
};

template<typename EnvListT = Loki::NullType>
class env : public env_gen<EnvListT> {
public:
    virtual ~env() {}

    template<typename T2>
    T2 * tryEnvOf(void) { return dynamic_cast<T2 *>(this); }

    template<typename T2>
    T2 & envOf(void) { return dynamic_cast<T2 &>(*this); }

    mem_allocrator_t t_allocrator() { return envOf<Test>().t_allocrator(); }
    void * t_alloc(size_t size) { return envOf<Test>().t_alloc(size); }
    char * t_strdup(const char * str) { return envOf<Test>().t_strdup(str); }
};

}

#endif
