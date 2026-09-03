add_executable(pixelgpu_demo 
    "./main.cpp"
    "../../src/pgpu_primitives.cpp"
    "../../src/pgpu_blitter.cpp"
)

target_include_directories(pixelgpu_demo PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

target_link_libraries(pixelgpu_demo PRIVATE 
    gpu_project_options 
    gpu_project_warnings
    $<LINK_LIBRARY:WHOLE_ARCHIVE,gpu_lib_mock_dumbfb>
)
