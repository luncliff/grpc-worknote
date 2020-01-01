
# grpc plugn: python3
find_program(grpc_python_plugin_path
    NAMES   grpc_python_plugin.exe grpc_python_plugin
    PATHS   ${PROJECT_SOURCE_DIR}/bin
            ${VCPKG_TOOL_DIR}/grpc
    # NO_DEFAULT_PATH 
)
file(RELATIVE_PATH short_path ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET} ${grpc_python_plugin_path})
message(STATUS "  python\t: ${short_path}")

add_custom_target(codegen_python
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto
                    --python_out=${PROJECT_SOURCE_DIR}/static
    COMMAND     ${proto_compiler_path} service.proto
                    --grpc_out=${PROJECT_SOURCE_DIR}/static
                    --plugin="protoc-gen-grpc=${grpc_python_plugin_path}"
)
