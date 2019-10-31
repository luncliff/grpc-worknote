
cmake_minimum_required(VERSION 3.13)
project(plugins LANGUAGES CXX VERSION 1.0)

find_package(protobuf 3.6   CONFIG REQUIRED)
find_package(gRPC           CONFIG REQUIRED)


# run protobuf compiler and generate files
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(grpc_codegen_targets)
include(report-project-info)

add_custom_command(
    COMMENT     "generating from 'service.proto' ..."
    OUTPUT      ${PROJECT_SOURCE_DIR}/static/service.pb.h
                ${PROJECT_SOURCE_DIR}/static/service.pb.cc
                ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.h
                ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.cc
    # WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto
                    --cpp_out=${PROJECT_SOURCE_DIR}/static
    COMMAND     ${proto_compiler_path} service.proto
                    --grpc_out=${PROJECT_SOURCE_DIR}/static
                    --plugin="protoc-gen-grpc=${grpc_cpp_plugin_path}"
)

add_library(plugin_core
    include/plugin_core.h
    static/service.pb.h

    src/libmain.cpp
    src/plugin_impl.h
    src/plugin_impl.cpp
    src/plugin_mock.cpp

    static/service.pb.h
    static/service.pb.cc
    static/service.grpc.pb.h
    static/service.grpc.pb.cc
)
# add_dependencies(plugin_core run_protoc_cpp)

set_target_properties(plugin_core
PROPERTIES
    CXX_STANDARD 17
    WINDOWS_EXPORT_ALL_SYMBOLS True
)

target_include_directories(plugin_core
PRIVATE
    ${PROJECT_SOURCE_DIR}/static
PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)

target_link_libraries(plugin_core
PUBLIC
    ${CMAKE_DL_LIBS}
    protobuf::libprotobuf # protobuf::libprotobuf-lite
    gRPC::grpc++ # gRPC::gpr gRPC::grpc gRPC::grpc_cronet
)

target_compile_definitions(plugin_core
PRIVATE
    PLATFORM_NAME=L"${CMAKE_SYSTEM_NAME}"
)

if(MSVC)
    target_compile_options(plugin_core
    PUBLIC
        /await
    )
    target_compile_definitions(plugin_core
    PUBLIC
        WIN32_LEAN_AND_MEAN NOMINMAX
        _WIN32_WINNT=0x0A00 # be explicit
    )
elseif(UNIX)
    target_compile_options(plugin_core
    PUBLIC
        -O0 -g
        -fPIC -stdlib=libc++ -fcoroutines-ts
    )
    target_link_libraries(plugin_core
    PUBLIC
        c++
    )
endif()

if(${CMAKE_TOOLCHAIN_FILE} MATCHES vcpkg.cmake)
    message(STATUS "using vcpkg: ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
    target_include_directories(plugin_core
    PUBLIC
        ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
    )
    target_link_directories(plugin_core
    PUBLIC
        ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
    )
endif()

add_executable(plugin_service
    static/service.pb.h
    static/service.grpc.pb.h
    
    tools/launcher.cpp
    tools/loader.h
    tools/loader.cpp

    tools/grpc_shared.h
    tools/grpc_completion_queue.cpp
    tools/grpc_service.cpp
    tools/grpc_handlers.cpp
)
set_target_properties(plugin_service
PROPERTIES
    CXX_STANDARD 17
)
target_include_directories(plugin_service
PRIVATE
    ${PROJECT_SOURCE_DIR}/static
)
target_link_libraries(plugin_service
PRIVATE
    plugin_core
)

#
# generate/install config & version info
#
include(CMakePackageConfigHelpers)
set(VERSION_FILE_PATH   ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}-config-version.cmake)
write_basic_package_version_file(${VERSION_FILE_PATH}
    VERSION             ${PROJECT_VERSION}
    COMPATIBILITY       SameMajorVersion
)
install(FILES           ${VERSION_FILE_PATH} 
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)

#
# proto definition
#
install(FILES           service.proto 
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)

#
# core module + launcher
#
install(FILES   ${PROJECT_SOURCE_DIR}/include/plugin_core.h
                ${PROJECT_SOURCE_DIR}/static/service.pb.h
                ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

# https://stackoverflow.com/questions/33991581/install-name-tool-to-update-a-executable-to-search-for-dylib-in-mac-os-x
install(TARGETS         plugin_core plugin_service
        EXPORT          ${PROJECT_NAME}-config
        RUNTIME  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

#
# ... testing comes here ...
#
if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    return()
endif()

enable_testing()
find_package(Catch2 CONFIG REQUIRED)

set(TEST_SUITE ${PROJECT_NAME}_test_suite)
add_executable(${TEST_SUITE}
    test/test_main.cpp
    test/test_env.cpp
    test/test_completion_queue.cpp
)

set_target_properties(${TEST_SUITE}
PROPERTIES
    CXX_STANDARD 17
)

target_include_directories(${TEST_SUITE}
PRIVATE
    ${PROJECT_SOURCE_DIR}/test
    ${PROJECT_SOURCE_DIR}/static
)

target_link_libraries(${TEST_SUITE}
PRIVATE
    plugin_core
    Catch2::Catch2
)
add_test(test_platform_abi  ${TEST_SUITE} "[abi]")
add_test(test_grpc_features ${TEST_SUITE} "[grpc]")
