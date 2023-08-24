cmake_minimum_required(VERSION 3.20)
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
set(include_dir_prefix ${my_cxx_toys_SOURCE_DIR}/include)
add_subdirectory(src/generic)
add_subdirectory(src/cpperclip)



# install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)

# include(CPack)
# set(CPACK_PACKAGE_NAME "MyPack")
# set(CPACK_GENERATOR "DEB")
