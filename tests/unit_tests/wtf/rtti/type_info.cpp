#include "../../../test_wtf.hpp"
#include <wtf/rtti/type_info.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace wtf;
using namespace test_wtf;

TEMPLATE_LIST_TEST_CASE("TypeInfo", "[wtf][rtti]", all_fp_types) {
    constexpr bool is_float = std::is_same_v<TestType, float>;
    using other_t           = std::conditional_t<is_float, double, float>;
    auto ti                 = rtti::wtf_typeid<TestType>();
    auto const_ti           = rtti::wtf_typeid<const TestType>();
    auto name               = type_traits::type_name_v<TestType>;

    SECTION("Ctors and assignment") {
        SECTION("Value ctor") {
            REQUIRE(ti.name() == name);
            REQUIRE(const_ti.name() == std::string("const ") + name);
        }
        SECTION("Copy ctor") {
            auto ti2 = ti;
            REQUIRE(ti2 == ti);

            auto const_ti2 = const_ti;
            REQUIRE(const_ti2 == const_ti);
        }
        SECTION("Move ctor") {
            auto ti2 = ti;
            auto ti3 = std::move(ti2);
            REQUIRE(ti3 == ti);

            auto const_ti2 = const_ti;
            auto const_ti3 = std::move(const_ti2);
            REQUIRE(const_ti3 == const_ti);
        }
        SECTION("Copy assignment") {
            auto ti2  = rtti::wtf_typeid<MyCustomFloat>();
            auto pti2 = &(ti2 = ti);
            REQUIRE(ti2 == ti);
            REQUIRE(pti2 == &ti2);

            auto const_ti2  = rtti::wtf_typeid<MyCustomFloat>();
            auto pconst_ti2 = &(const_ti2 = const_ti);
            REQUIRE(const_ti2 == const_ti);
            REQUIRE(pconst_ti2 == &const_ti2);
        }
        SECTION("Move assignment") {
            auto ti2  = rtti::wtf_typeid<MyCustomFloat>();
            auto ti3  = ti;
            auto pti2 = &(ti2 = std::move(ti3));
            REQUIRE(ti2 == ti);
            REQUIRE(pti2 == &ti2);

            auto const_ti2  = rtti::wtf_typeid<MyCustomFloat>();
            auto const_ti3  = const_ti;
            auto pconst_ti2 = &(const_ti2 = std::move(const_ti3));
            REQUIRE(const_ti2 == const_ti);
            REQUIRE(pconst_ti2 == &const_ti2);
        }
    }

    SECTION("name") {
        REQUIRE(ti.name() == name);
        REQUIRE(const_ti.name() == std::string("const ") + name);
    }

    SECTION("precision") {
        auto corr = type_traits::precision_v<TestType>;
        REQUIRE(ti.precision() == corr);
        REQUIRE(const_ti.precision() == corr);
    }

    SECTION("operator==") {
        REQUIRE(ti == rtti::wtf_typeid<TestType>());
        REQUIRE(const_ti == rtti::wtf_typeid<const TestType>());
        REQUIRE_FALSE(ti == const_ti);
        REQUIRE_FALSE(ti == rtti::wtf_typeid<other_t>());
        REQUIRE_FALSE(const_ti == rtti::wtf_typeid<other_t>());
    }

    SECTION("operator!=") {
        REQUIRE_FALSE(ti != rtti::wtf_typeid<TestType>());
        REQUIRE_FALSE(const_ti != rtti::wtf_typeid<const TestType>());
        REQUIRE(ti != const_ti);
        REQUIRE(ti != rtti::wtf_typeid<other_t>());
        REQUIRE(const_ti != rtti::wtf_typeid<other_t>());
    }
}

TEST_CASE("is_implicitly_convertible") {
    using types = all_fp_types;

    auto tf = rtti::wtf_typeid<float>();
    auto td = rtti::wtf_typeid<double>();
    auto tc = rtti::wtf_typeid<MyCustomFloat>();

    auto ctf = rtti::wtf_typeid<const float>();
    auto ctd = rtti::wtf_typeid<const double>();
    auto ctc = rtti::wtf_typeid<const MyCustomFloat>();

    REQUIRE(rtti::is_implicitly_convertible<types>(tf, tf));
    REQUIRE(rtti::is_implicitly_convertible<types>(tf, td));
    REQUIRE(rtti::is_implicitly_convertible<types>(tf, tc));
    REQUIRE(rtti::is_implicitly_convertible<types>(tf, ctf));
    REQUIRE(rtti::is_implicitly_convertible<types>(tf, ctd));
    REQUIRE(rtti::is_implicitly_convertible<types>(tf, ctc));

    REQUIRE(rtti::is_implicitly_convertible<types>(td, tf));
    REQUIRE(rtti::is_implicitly_convertible<types>(td, td));
    REQUIRE(rtti::is_implicitly_convertible<types>(td, tc));
    REQUIRE(rtti::is_implicitly_convertible<types>(td, ctf));
    REQUIRE(rtti::is_implicitly_convertible<types>(td, ctd));
    REQUIRE(rtti::is_implicitly_convertible<types>(td, ctc));

    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(tc, tf));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(tc, td));
    REQUIRE(rtti::is_implicitly_convertible<types>(tc, tc));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(tc, ctf));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(tc, ctd));
    REQUIRE(rtti::is_implicitly_convertible<types>(tc, ctc));

    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctf, tf));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctf, td));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctf, tc));
    REQUIRE(rtti::is_implicitly_convertible<types>(ctf, ctf));
    REQUIRE(rtti::is_implicitly_convertible<types>(ctf, ctd));
    REQUIRE(rtti::is_implicitly_convertible<types>(ctf, ctc));

    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctd, tf));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctd, td));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctd, tc));
    REQUIRE(rtti::is_implicitly_convertible<types>(ctd, ctf));
    REQUIRE(rtti::is_implicitly_convertible<types>(ctd, ctd));
    REQUIRE(rtti::is_implicitly_convertible<types>(ctd, ctc));

    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctc, tf));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctc, td));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctc, tc));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctc, ctf));
    REQUIRE_FALSE(rtti::is_implicitly_convertible<types>(ctc, ctd));
    REQUIRE(rtti::is_implicitly_convertible<types>(ctc, ctc));
}