cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(FetchContent)
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

# Fetch project_options
set(PROJECT_OPTIONS_VERSION "v0.41.0")
FetchContent_Declare(
  _project_options
  URL "https://github.com/aminya/project_options/archive/refs/tags/${PROJECT_OPTIONS_VERSION}.zip")
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

include("./Features.cmake")

option(ENABLE_VCPKG "Use vcpkg to install dependencies" ${WIN32})

project(
  pixelgpu
  VERSION 0.1.0
  DESCRIPTION "Software 2D GPU & Graphics Rasterizer from scratch in C++20"
  LANGUAGES CXX C)

include("./cmake/Dependencies.cmake")

if(FEATURE_TESTS)
  enable_testing()
  add_library(catch2_test_common INTERFACE)
  target_find_dependencies(catch2_test_common INTERFACE_CONFIG Catch2)
  target_link_libraries(catch2_test_common INTERFACE Catch2::Catch2 Catch2::Catch2WithMain)
  target_compile_definitions(catch2_test_common INTERFACE DO_NOT_USE_WMAIN)
  include(Catch)
endif()

project_options(
  PREFIX "gpu"
  ENABLE_CACHE
)

add_subdirectory(./libs/)
add_subdirectory(./apps/)
