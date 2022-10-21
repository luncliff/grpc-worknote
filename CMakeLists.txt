cmake_minimum_required(VERSION 3.18)
project(worknote LANGUAGES CXX VERSION 0.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# see https://cmake.org/cmake/help/latest/module/FindProtobuf.html
find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# see 'codegen_xx.cmake' files in cmake/
# find protobuf compiler in vcpkg
find_program(protoc_exe NAMES protoc REQUIRED)

list(APPEND hdrs
    ${PROJECT_SOURCE_DIR}/static/service.pb.h
    ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.h
    ${PROJECT_SOURCE_DIR}/static/service.pb.cc
)

add_library(protocol0 STATIC
    signal.cpp
    serve_queue.cpp
    serve.cpp
    ${hdrs}
    ${PROJECT_SOURCE_DIR}/static/service.pb.cc
    ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.cc
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
        /std:c++17 /await
    )
endif()


add_executable(service0
    rpc_service.cpp  
)

if(WIN32)
    target_compile_definitions(service0
    PRIVATE
        _WIN32_WINNT=0x0A00 # be explicit
    )
endif()

install(TARGETS protocol0 service0
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
