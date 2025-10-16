#include "../../../test_wtf.hpp"
#include <wtf/type_traits/type_name.hpp>
#include <wtf/type_traits/type_traits.hpp>

using namespace wtf::type_traits;
using namespace test_wtf;

WTF_REGISTER_FP_TYPE(MyCustomFloat);

TEST_CASE("TypeName") {
    REQUIRE(TypeName<float>::value == std::string("float"));
    REQUIRE(type_name_v<float> == std::string("float"));
    REQUIRE(TypeName<double>::value == std::string("double"));
    REQUIRE(type_name_v<double> == std::string("double"));
    REQUIRE(TypeName<long double>::value == std::string("long double"));
    REQUIRE(type_name_v<long double> == std::string("long double"));

    REQUIRE(TypeName<MyCustomFloat>::value == std::string("MyCustomFloat"));
    REQUIRE(type_name_v<MyCustomFloat> == std::string("MyCustomFloat"));
}