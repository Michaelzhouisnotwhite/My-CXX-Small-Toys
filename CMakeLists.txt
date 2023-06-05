cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 17)
project(my_cxx_toys)
include(FetchContent)
option(TIMER_VERBOSE "is timer verbose" OFF)
option(BUILD_TOY_EXAMPLE "build example" OFF)

function(fetch_content_check content_name)
    # FetchContent_GetProperties(${content_name})
    FetchContent_MakeAvailable(${content_name})

    # if(NOT ${content_name}_BINARY_DIR)
    # FetchContent_MakeAvailable(${content_name})
    # else()
    message("---- ${content_name}_SOURCE_DIR is found in ${${content_name}_SOURCE_DIR}")
    message("---- ${content_name}_BINARY_DIR is found in ${${content_name}_BINARY_DIR}")

    # endif()
endfunction()

if(${BUILD_TOY_EXAMPLE})
    FetchContent_Declare(
        cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr
        GIT_TAG 2553fc41450301cd09a9271c8d2c3e0cf3546b73
        OVERRIDE_FIND_PACKAGE
    )
    message("---- example ready to build")
    fetch_content_check(cpr)
    add_subdirectory(example)
endif()

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG 10.0.0
    OVERRIDE_FIND_PACKAGE
)
fetch_content_check(fmt)

# find_package(fmt REQUIRED)
# add_subdirectory(3dparty)
if(${TIMER_VERBOSE})
    message("---- timer verbose is on")
    add_compile_definitions(TIMER_VERBOSE=1)
else()
    add_compile_definitions(TIMER_VERBOSE=0)
endif()

file(GLOB src_files src/*.cpp)
add_library(${PROJECT_NAME} ${src_files})
add_library(my_cxx_toys::toy ALIAS my_cxx_toys)
target_precompile_headers(${PROJECT_NAME} PUBLIC include/toy/pch.hpp)
target_link_libraries(${PROJECT_NAME} fmt::fmt)
target_include_directories(${PROJECT_NAME} PUBLIC "include")
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)

include(CPack)
set(CPACK_PACKAGE_NAME "MyPack")
set(CPACK_GENERATOR "DEB")
