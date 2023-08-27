project(cpperclip)
FetchContent_Declare(
    clip
    GIT_REPOSITORY https://github.com/dacap/clip.git
    GIT_TAG main
    OVERRIDE_FIND_PACKAGE
)

# Disable clip examples and tests
set(CLIP_EXAMPLES OFF CACHE BOOL "Compile clip examples")
set(CLIP_TESTS OFF CACHE BOOL "Compile clip tests")
fetch_content_check(clip)
set(INTERVAL_TIME 300 CACHE STRING "get clipboard interval time")
file(GLOB srcs *.[hc]pp)
add_library(cpperclip ${srcs})
target_include_directories(cpperclip PUBLIC ${include_dir_prefix} ${clip_SOURCE_DIR})
target_link_libraries(cpperclip PUBLIC clip)
target_precompile_headers(cpperclip PRIVATE "${include_dir_prefix}/private/pch.hpp")
target_link_libraries(cpperclip PUBLIC fmt::fmt)
target_compile_definitions(cpperclip PRIVATE INTERVAL_TIME=${INTERVAL_TIME})
add_library(toy::cpperclip ALIAS cpperclip)
