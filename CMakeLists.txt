cmake_minimum_required(VERSION 3.14)
project(WeaklyTypedFloat VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library target
add_library(weakly_typed_float
    src/weakly_typed_float.cpp
)

# Add include directory
target_include_directories(weakly_typed_float
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Installation rules
install(TARGETS weakly_typed_float
    EXPORT WeaklyTypedFloatTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
)

# Export targets
install(EXPORT WeaklyTypedFloatTargets
    FILE WeaklyTypedFloatTargets.cmake
    NAMESPACE WeaklyTypedFloat::
    DESTINATION lib/cmake/WeaklyTypedFloat
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/WeaklyTypedFloatConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/WeaklyTypedFloatConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/WeaklyTypedFloatConfig.cmake"
    INSTALL_DESTINATION lib/cmake/WeaklyTypedFloat
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/WeaklyTypedFloatConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/WeaklyTypedFloatConfigVersion.cmake"
    DESTINATION lib/cmake/WeaklyTypedFloat
)
