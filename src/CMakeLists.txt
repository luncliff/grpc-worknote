
# we can't sure the source code are generated at this moment
# always create them with 'service.proto'
add_custom_command(
    COMMENT     "generating from 'service.proto' ..."
    OUTPUT      ${PROJECT_SOURCE_DIR}/static/service.pb.h
                ${PROJECT_SOURCE_DIR}/static/service.pb.cc
                ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.h
                ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.cc
    # WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} ${PROJECT_SOURCE_DIR}/service.proto 
                    --proto_path=${PROJECT_SOURCE_DIR}
                    --cpp_out=${PROJECT_SOURCE_DIR}/static
    COMMAND     ${proto_compiler_path} ${PROJECT_SOURCE_DIR}/service.proto 
                    --proto_path=${PROJECT_SOURCE_DIR}
                    --grpc_out=${PROJECT_SOURCE_DIR}/static
                    --plugin="protoc-gen-grpc=${grpc_cpp_plugin_path}"
)

add_library(rpc_impl SHARED
    signal.cpp
    serve_queue.cpp serve.cpp
    ${PROJECT_SOURCE_DIR}/static/service.pb.h
    ${PROJECT_SOURCE_DIR}/static/service.pb.cc
    ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.h
    ${PROJECT_SOURCE_DIR}/static/service.grpc.pb.cc
)

set_target_properties(rpc_impl
PROPERTIES
    CXX_STANDARD 17
)

target_include_directories(rpc_impl
PRIVATE
    # ${VCPKG_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/static
PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)

# target_link_directories(rpc_impl
# PRIVATE
#     ${VCPKG_LIB_DIRS}
# )

target_link_libraries(rpc_impl
PRIVATE
    vcpkg_meta
PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
)

target_compile_definitions(rpc_impl
PRIVATE
    ${VCPKG_DEFINITIONS}
    PLATFORM_NAME=L"${CMAKE_SYSTEM_NAME}"
)

if(WIN32)
    set_target_properties(rpc_impl
    PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS True
    )
    target_compile_definitions(rpc_impl
    PRIVATE
        WIN32_LEAN_AND_MEAN NOMINMAX
        _WIN32_WINNT=0x0A00 # be explicit
    )
elseif(APPLE)
    target_link_libraries(rpc_impl
    PRIVATE
        c++
    )
endif()

# using C++ 20 Coroutines
if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    target_compile_options(rpc_impl
    PRIVATE
        -std=c++2a -fcoroutines-ts -stdlib=libc++
    )
elseif(MSVC)
    target_compile_options(rpc_impl
    PRIVATE
        /std:c++latest /await
    )
endif()

