#include "../../../test_wtf.hpp"
#include <wtf/type_traits/tuple_append.hpp>

using namespace wtf::type_traits;

TEST_CASE("TuppleAppend") {
    using tuple0 = std::tuple<>;
    using tuple1 = std::tuple<int>;
    using tuple2 = std::tuple<double, float>;

    SECTION("empty + empty") {
        using result = tuple_append_t<tuple0, tuple0>;
        REQUIRE(std::is_same_v<result, tuple0>);
    }

    SECTION("empty + non-empty") {
        using result = tuple_append_t<tuple0, tuple1>;
        REQUIRE(std::is_same_v<result, tuple1>);
    }

    SECTION("non-empty + empty") {
        using result = tuple_append_t<tuple2, tuple0>;
        REQUIRE(std::is_same_v<result, tuple2>);
    }

    SECTION("non-empty + non-empty") {
        using result12   = tuple_append_t<tuple1, tuple2>;
        using expected12 = std::tuple<int, double, float>;
        REQUIRE(std::is_same_v<result12, expected12>);

        using result21   = tuple_append_t<tuple2, tuple1>;
        using expected21 = std::tuple<double, float, int>;
        REQUIRE(std::is_same_v<result21, expected21>);
    }
}