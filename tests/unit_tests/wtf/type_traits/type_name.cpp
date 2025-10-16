#include "../../../test_wtf.hpp"
#include <wtf/type_traits/type_name.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace wtf::type_traits;
using namespace test_wtf;

WTF_REGISTER_FP_TYPE(MyCustomFloat);

TEST_CASE("TypeName") {
    REQUIRE(TypeName<float>::value == std::string("float"));
    REQUIRE(TypeName<double>::value == std::string("double"));
    REQUIRE(TypeName<long double>::value == std::string("long double"));
    REQUIRE(TypeName<test_wtf::MyCustomFloat>::value ==
            std::string("MyCustomFloat"));
}