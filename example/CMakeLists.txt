project(example)
file(GLOB example_srcs "*.cpp")

foreach (example_src IN LISTS example_srcs)
    get_filename_component(example_proj_name ${example_src} NAME)
    message("----${example_proj_name}.exe in example created")
    add_executable(${example_proj_name} ${example_src})
    target_link_libraries(${example_proj_name} PRIVATE fmt::fmt my_cxx_toys)
    target_include_directories(${example_proj_name} PRIVATE "../include")
endforeach ()

