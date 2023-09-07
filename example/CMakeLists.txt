project(example)
file(GLOB example_srcs "*.cpp")
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr
    GIT_TAG 2553fc41450301cd09a9271c8d2c3e0cf3546b73
    OVERRIDE_FIND_PACKAGE
)
message("---- example ready to build")
fetch_content_check(cpr)
add_compile_definitions(TIMER_VERBOSE=1)
foreach(example_src IN LISTS example_srcs)
    get_filename_component(example_proj_name ${example_src} NAME_WLE)

    set(sub_proj_name example_${example_proj_name})
    message("----project name: ${sub_proj_name} in example is created")
    add_executable(${sub_proj_name} ${example_src})
    target_link_libraries(${sub_proj_name} PRIVATE fmt::fmt toy::generic toy::cpperclip cpr::cpr)

    target_precompile_headers(${sub_proj_name} PRIVATE pch.h)
    target_include_directories(${sub_proj_name} PRIVATE "../include")
    # add_custom_command(TARGET ${sub_proj_name} POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -DBINARY_DIR=${CMAKE_BINARY_DIR} -DDESTINATION="${CMAKE_BINARY_DIR}/example" -P ${PROJECT_SOURCE_DIR}/CopyDLLS.cmake)
endforeach()
