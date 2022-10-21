cmake_minimum_required(VERSION 3.18)
project(worknote LANGUAGES CXX VERSION 0.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CONAN_CMAKE_SILENT_OUTPUT true)
include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# see https://cmake.org/cmake/help/latest/module/FindProtobuf.html
find_program(Protobuf_PROTOC_EXECUTABLE NAMES protoc REQUIRED)
find_program(GRPC_CPP_PLUGIN_PROGRAM NAMES grpc_cpp_plugin REQUIRED)
find_package(protobuf REQUIRED)
find_package(gRPC REQUIRED)

# run codegen for manual source update
add_custom_target(codegen_cpp
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    SOURCES service.proto
    COMMAND ${Protobuf_PROTOC_EXECUTABLE} --version
    COMMAND ${Protobuf_PROTOC_EXECUTABLE} service.proto --cpp_out=src
    COMMAND ${Protobuf_PROTOC_EXECUTABLE} service.proto --cpp_out=src --grpc_out=src --plugin="protoc-gen-grpc=${GRPC_CPP_PLUGIN_PROGRAM}"
)

list(APPEND codegen_hdrs
    src/service.pb.h
    src/service.grpc.pb.h
)
list(APPEND codegen_srcs
    src/service.pb.cc
    src/service.grpc.pb.cc
)

add_custom_command(OUTPUT ${codegen_hdrs} ${codegen_srcs}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE} service.proto --cpp_out=src
    COMMAND ${Protobuf_PROTOC_EXECUTABLE} service.proto --cpp_out=src --grpc_out=src --plugin="protoc-gen-grpc=${GRPC_CPP_PLUGIN_PROGRAM}"
)

add_library(protocol0 STATIC
    src/signal.cpp
    src/serve_queue.cpp
    src/serve.cpp
    ${codegen_hdrs}
    ${codegen_srcs}
)

set_target_properties(protocol0
    PROPERTIES
    PUBLIC_HEADER "${hdrs}"
)

target_include_directories(protocol0
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/static>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(protocol0
    PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
)

if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    target_compile_options(protocol0
        PRIVATE
        -fcoroutines-ts -stdlib=libc++
    )
elseif(MSVC)
    target_compile_options(protocol0
        PRIVATE
        /Zc:__cplusplus /await
    )
endif()

add_executable(service0
    tools/rpc_service.cpp
)

target_link_libraries(service0
    PRIVATE
    protocol0
)

install(TARGETS protocol0 service0
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
