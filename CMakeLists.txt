
cmake_minimum_required(VERSION 3.14)
project(worknote LANGUAGES CXX VERSION 0.2)

find_package(protobuf 3.10      CONFIG REQUIRED)
find_package(gRPC     1.23      CONFIG REQUIRED)

# run protobuf compiler and generate files
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# new target 'vcpkg_meta'
#   it makes libraries in vcpkg visible through 'target_link_libraries'
include(vcpkg_meta)

# see 'codegen_xx.cmake' files in cmake/
#   * codegen_cpp
#   * codegen_csharp
#   * codegen_python
#   * codegen_nodejs
include(codegen)

add_subdirectory(src)

return()

#
# ... testing comes here ...
#
if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    return()
endif()

enable_testing()
add_subdirectory(test)
