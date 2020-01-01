
# grpc plugn: node.js
find_program(grpc_node_plugin_path
    NAMES   grpc_node_plugin.exe grpc_node_plugin
    PATHS   ${PROJECT_SOURCE_DIR}/bin
            ${VCPKG_TOOL_DIR}/grpc
    # NO_DEFAULT_PATH 
)
file(RELATIVE_PATH short_path ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET} ${grpc_node_plugin_path})
message(STATUS "  nodejs\t: ${short_path}")

add_custom_target(codegen_nodejs
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto 
                    --grpc_out=${PROJECT_SOURCE_DIR}/static
                    --plugin="protoc-gen-grpc=${grpc_node_plugin_path}"
)
