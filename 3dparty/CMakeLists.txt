file(GLOB sub_dirs *)

foreach(sub_dir ${sub_dirs})    
    add_subdirectory(sub_dir)
endforeach()