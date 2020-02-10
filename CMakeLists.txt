
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
set(VCPKG_TOOL_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools)
# find protobuf compiler in vcpkg
find_program(proto_compiler_path
    NAMES   protoc.exe protoc
    PATHS   ${VCPKG_TOOL_DIR}/protobuf
    # NO_DEFAULT_PATH 
)
file(RELATIVE_PATH short_path ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET} ${proto_compiler_path})
message(STATUS "using protoc\t: ${short_path}")
include(codegen_cpp)
include(codegen_python)
include(codegen_nodejs)
include(codegen_csharp)
if (DEFINED $ENV{GOPATH})
    include(codegen_go)
endif()

add_subdirectory(src)
add_subdirectory(tools)

install(TARGETS     rpc_impl 
                    rpc_service
        RUNTIME  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        # LIBRARY  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        # ARCHIVE  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
