include_guard()
include(FetchContent)

function(get_dependencies)
    foreach(depend_i ${ARGN})
        message(STATUS "Fetching dependency: ${depend_i}")
        include("cmake/dependencies/${depend_i}.cmake")
    endforeach()

    FetchContent_MakeAvailable(${ARGN})
endfunction()