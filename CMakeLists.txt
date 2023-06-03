cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 17)
project(my_cxx_toys)
option(TIMER_VERBOSE "is timer verbose" OFF)
option(BUILD_EXAMPLE "build example" ON)

if(${BUILD_EXAMPLE})
    add_subdirectory(example)
endif()

add_subdirectory(3dparty)
if (${TIMER_VERBOSE})
    message("----timer verbose is on")
    add_compile_definitions(TIMER_VERBOSE=1)
else ()
    add_compile_definitions(TIMER_VERBOSE=0)
endif ()

file(GLOB src_files src/*.cpp)
add_library(${PROJECT_NAME} ${src_files})
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.hpp PUBLIC include/toy/pch.hpp)
target_link_libraries(${PROJECT_NAME} fmt::fmt)
target_include_directories(${PROJECT_NAME} PUBLIC "include")

