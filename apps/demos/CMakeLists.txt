    add_executable(pixelgpu_demo "./main.cpp")

    target_link_libraries(pixelgpu_demo PRIVATE 
        gpu_project_options 
        gpu_project_warnings
        $<LINK_LIBRARY:WHOLE_ARCHIVE,gpu_lib_mock_dumbfb>
    )
