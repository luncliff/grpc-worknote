cmake_minimum_required(VERSION 3.14)

project(grpc_async LANGUAGES CXX)

find_package(gRPC   CONFIG REQUIRED)

if(NOT ${CMAKE_TOOLCHAIN_FILE} MATCHES vcpkg.cmake)
    message(FATAL_ERROR "This project expects vcpkg toolchain")
endif()
message(STATUS "using vcpkg: ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# gRPC C++
add_library(grpc_async_core
    include/service_core.h
    src/libmain.cpp
    src/grpc_service.cpp
    src/process.cpp
)

target_include_directories(grpc_async_core
PRIVATE
    ${PROJECT_SOURCE_DIR}/static
    ${PROJECT_SOURCE_DIR}/external/cpp-subprocess
PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
)

target_link_directories(grpc_async_core
PUBLIC
    ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
)

target_link_libraries(grpc_async_core
PUBLIC
    gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc_cronet
)

if(MSVC)
    target_compile_options(grpc_async_core
    PUBLIC
        /W4 /await
    )
endif()

# run protobuf compiler and generate files
include(grpc_codegen_targets)
add_dependencies(grpc_async_core generate_grpc_files_cpp)

target_sources(grpc_async_core
PRIVATE
    static/service.grpc.pb.h
    static/service.grpc.pb.cc
    static/service.pb.h
    static/service.pb.cc
)

set_target_properties(grpc_async_core
PROPERTIES
    CXX_STANDARD 20
    WINDOWS_EXPORT_ALL_SYMBOLS True
)
target_compile_definitions(grpc_async_core
PRIVATE
    WIN32_LEAN_AND_MEAN NOMINMAX
    _WIN32_WINNT=0x0A00
)

# ... testing comes here ...
enable_testing()
find_package(Catch2 CONFIG REQUIRED)

set(TEST_SUITE ${PROJECT_NAME}_test_suite)
add_executable(${TEST_SUITE}
    test/test_main.cpp
)

set_target_properties(${TEST_SUITE}
PROPERTIES
    CXX_STANDARD 20
)

target_include_directories(${TEST_SUITE}
PRIVATE
    ${PROJECT_SOURCE_DIR}/test
    ${PROJECT_SOURCE_DIR}/static
)

target_link_libraries(${TEST_SUITE}
PRIVATE
    grpc_async_core 
    ${CMAKE_DL_LIBS}
    Catch2::Catch2
)

add_test(test_basic_loading ${TEST_SUITE})
