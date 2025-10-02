#include "weakly_typed_float/weakly_typed_float.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

using namespace weakly_typed_float;

void test_default_constructor() {
    WeaklyTypedFloat wtf;
    assert(wtf.value() == 0.0);
    std::cout << "✓ Default constructor test passed" << std::endl;
}

void test_float_constructor() {
    float f = 3.14f;
    WeaklyTypedFloat wtf(f);
    assert(std::abs(wtf.value() - 3.14) < 0.001);
    std::cout << "✓ Float constructor test passed" << std::endl;
}

void test_double_constructor() {
    double d = 2.71828;
    WeaklyTypedFloat wtf(d);
    assert(std::abs(wtf.value() - 2.71828) < 0.00001);
    std::cout << "✓ Double constructor test passed" << std::endl;
}

void test_copy_constructor() {
    WeaklyTypedFloat wtf1(3.14);
    WeaklyTypedFloat wtf2(wtf1);
    assert(wtf1.value() == wtf2.value());
    std::cout << "✓ Copy constructor test passed" << std::endl;
}

void test_assignment_operator() {
    WeaklyTypedFloat wtf1(3.14);
    WeaklyTypedFloat wtf2;
    wtf2 = wtf1;
    assert(wtf1.value() == wtf2.value());
    std::cout << "✓ Assignment operator test passed" << std::endl;
}

void test_addition() {
    WeaklyTypedFloat wtf1(3.0);
    WeaklyTypedFloat wtf2(2.0);
    WeaklyTypedFloat result = wtf1 + wtf2;
    assert(result.value() == 5.0);
    std::cout << "✓ Addition test passed" << std::endl;
}

void test_subtraction() {
    WeaklyTypedFloat wtf1(5.0);
    WeaklyTypedFloat wtf2(2.0);
    WeaklyTypedFloat result = wtf1 - wtf2;
    assert(result.value() == 3.0);
    std::cout << "✓ Subtraction test passed" << std::endl;
}

void test_multiplication() {
    WeaklyTypedFloat wtf1(3.0);
    WeaklyTypedFloat wtf2(4.0);
    WeaklyTypedFloat result = wtf1 * wtf2;
    assert(result.value() == 12.0);
    std::cout << "✓ Multiplication test passed" << std::endl;
}

void test_division() {
    WeaklyTypedFloat wtf1(12.0);
    WeaklyTypedFloat wtf2(4.0);
    WeaklyTypedFloat result = wtf1 / wtf2;
    assert(result.value() == 3.0);
    std::cout << "✓ Division test passed" << std::endl;
}

void test_comparison() {
    WeaklyTypedFloat wtf1(3.0);
    WeaklyTypedFloat wtf2(3.0);
    WeaklyTypedFloat wtf3(5.0);
    
    assert(wtf1 == wtf2);
    assert(wtf1 != wtf3);
    assert(wtf1 < wtf3);
    assert(wtf1 <= wtf2);
    assert(wtf3 > wtf1);
    assert(wtf2 >= wtf1);
    std::cout << "✓ Comparison tests passed" << std::endl;
}

void test_as_conversion() {
    WeaklyTypedFloat wtf(3.14159);
    float f = wtf.as<float>();
    double d = wtf.as<double>();
    assert(std::abs(f - 3.14159f) < 0.001f);
    assert(std::abs(d - 3.14159) < 0.00001);
    std::cout << "✓ Type conversion test passed" << std::endl;
}

int main() {
    std::cout << "Running WeaklyTypedFloat tests..." << std::endl;
    
    try {
        test_default_constructor();
        test_float_constructor();
        test_double_constructor();
        test_copy_constructor();
        test_assignment_operator();
        test_addition();
        test_subtraction();
        test_multiplication();
        test_division();
        test_comparison();
        test_as_conversion();
        
        std::cout << "\n✓ All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "✗ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
