
# grpc plugin: c++
find_program(grpc_cpp_plugin_path
    NAMES   grpc_cpp_plugin.exe grpc_cpp_plugin
    PATHS   ${VCPKG_TOOL_DIR}/grpc
    # NO_DEFAULT_PATH 
)
file(RELATIVE_PATH short_path ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET} ${grpc_cpp_plugin_path})
message(STATUS "  c++   \t: ${short_path}")

# code generation: c++
add_custom_target(codegen_cpp
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto
                    --cpp_out=${PROJECT_SOURCE_DIR}/static
    COMMAND     ${proto_compiler_path} service.proto
                    --grpc_out=${PROJECT_SOURCE_DIR}/static
                    --plugin="protoc-gen-grpc=${grpc_cpp_plugin_path}"
)
