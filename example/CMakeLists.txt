project(example)
file(GLOB example_srcs "*.cpp")

# find_package(cpr REQUIRED)
# fetch_content_check(cpr)
foreach(example_src IN LISTS example_srcs)
    get_filename_component(example_proj_name ${example_src} NAME_WLE)
    message("----${example_proj_name}.exe in example is created")
    add_executable(${example_proj_name} ${example_src})
    target_link_libraries(${example_proj_name} PRIVATE fmt::fmt my_cxx_toys cpr::cpr)

    target_precompile_headers(${example_proj_name} PRIVATE pch.h)
    target_include_directories(${example_proj_name} PRIVATE "../include")
    add_custom_command(TARGET ${example_proj_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -DBINARY_DIR=${CMAKE_BINARY_DIR} -DDESTINATION="${CMAKE_BINARY_DIR}/example" -P ${PROJECT_SOURCE_DIR}/CopyDLLS.cmake)
endforeach()
