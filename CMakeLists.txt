cmake_minimum_required(VERSION 3.14)

project(grpc_plugins LANGUAGES CXX)

find_package(protobuf 3.6 CONFIG REQUIRED)
find_package(gRPC   CONFIG REQUIRED)

if(NOT ${CMAKE_TOOLCHAIN_FILE} MATCHES vcpkg.cmake)
    message(FATAL_ERROR "This project expects vcpkg toolchain")
endif()
message(STATUS "using vcpkg: ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# gRPC C++
add_library(plugin_core SHARED
    include/plugin_core.h
    src/libmain.cpp

    src/plugin_impl.h
    src/plugin_impl.cpp
)

target_include_directories(plugin_core
PRIVATE
    ${PROJECT_SOURCE_DIR}/static
PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
)

target_link_directories(plugin_core
PUBLIC
    ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
)

target_link_libraries(plugin_core
PUBLIC
    ${CMAKE_DL_LIBS}
    protobuf::libprotobuf protobuf::libprotobuf-lite
)

if(MSVC)
    target_compile_options(plugin_core
    PUBLIC
        /await
    )
endif()

# run protobuf compiler and generate files
include(grpc_codegen_targets)
add_dependencies(plugin_core run_protoc_cpp)

add_custom_command(
    COMMENT     "generating grpc++ files"
    OUTPUT      ${PROJECT_SOURCE_DIR}/static/service.pb.h
                ${PROJECT_SOURCE_DIR}/static/service.pb.cc
                ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.h
                ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.cc
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto
                    --cpp_out=${PROJECT_SOURCE_DIR}/static
    COMMAND     ${proto_compiler_path} service.proto
                    --grpc_out=${PROJECT_SOURCE_DIR}/static
                    --plugin="protoc-gen-grpc=${grpc_cpp_plugin_path}"
)

target_sources(plugin_core
PRIVATE
    static/service.pb.h
    static/service.pb.cc
)

set_target_properties(plugin_core
PROPERTIES
    CXX_STANDARD 20
    WINDOWS_EXPORT_ALL_SYMBOLS True
)

target_compile_definitions(plugin_core
PRIVATE
    PLATFORM_NAME=L"${CMAKE_SYSTEM_NAME}"
)

if(WIN32)
    target_compile_definitions(plugin_core
    PUBLIC
        WIN32_LEAN_AND_MEAN NOMINMAX
        _WIN32_WINNT=0x0A00
    )
endif()


add_executable(plugin_launcher
    tools/launcher.cpp
    
    static/service.grpc.pb.h
    static/service.grpc.pb.cc

    tools/loader.h
    tools/loader.cpp

    tools/grpc_shared.h
    tools/grpc_completion_queue.cpp
    tools/grpc_service.cpp
    tools/grpc_handlers.cpp
)

set_target_properties(plugin_launcher
PROPERTIES
    CXX_STANDARD 20
)

target_include_directories(plugin_launcher
PRIVATE
    ${PROJECT_SOURCE_DIR}/static
)

target_link_libraries(plugin_launcher
PRIVATE
    plugin_core
    gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc_cronet
)

# ... testing comes here ...
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    return()
endif()
return()

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
    plugin_core 
    ${CMAKE_DL_LIBS}
    Catch2::Catch2
)

add_test(test_basic ${TEST_SUITE})
