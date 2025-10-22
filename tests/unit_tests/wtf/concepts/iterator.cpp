#include "../../../test_wtf.hpp"
#include <wtf/concepts/iterator.hpp>

using namespace wtf::concepts;

TEMPLATE_LIST_TEST_CASE("FPIterator", "[wtf][concepts]",
                        test_wtf::all_fp_types) {
    using vector_type        = std::vector<TestType>;
    using vector_iterator    = typename vector_type::iterator;
    using pointer_type       = TestType*;
    using const_pointer_type = const TestType*;

    STATIC_REQUIRE(FPIterator<vector_iterator>);
    STATIC_REQUIRE(FPIterator<pointer_type>);
    STATIC_REQUIRE(FPIterator<const_pointer_type>);

    STATIC_REQUIRE_FALSE(FPIterator<int>);
}