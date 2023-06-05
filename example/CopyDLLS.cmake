function(copy_dlls binary_dir destination)
    message("---- copying dlls from ${binary_dir} to ${destination}")
    file(GLOB_RECURSE DLLS "${binary_dir}/*.dll")

    foreach(dll ${DLLS})
        message("----       ${dll}; ")
        get_filename_component(dll_name_with_ext ${dll} NAME)
        file(COPY ${dll} DESTINATION ${destination})
    endforeach()
endfunction(copy_dlls)

copy_dlls(${BINARY_DIR} ${DESTINATION})

# file(GLOB_RECURSE dlls ${CMAKE_BINARY_DIR}/*/*.dll.a)

# foreach(dll ${dlls})
# message("----       ${dll}; ")
# get_filename_component(dll_name_with_ext ${dll} NAME)
# file(COPY_FILE ${dll} ${PROJECT_BINARY_DIR} ONLY_IF_DIFFERENT)
# endforeach()
