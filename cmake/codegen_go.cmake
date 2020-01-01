
# $env:GOPATH=pwd
# go get -u github.com/golang/protobuf/protoc-gen-go
find_program(grpc_go_plugin_path
    NAMES   protoc-gen-go.exe protoc-gen-go
    PATHS   $ENV{GOPATH}/bin
            ${PROJECT_SOURCE_DIR}/bin
            ${VCPKG_TOOL_DIR}/grpc
    NO_DEFAULT_PATH 
)
file(RELATIVE_PATH short_path ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET} ${grpc_go_plugin_path})
message(STATUS "  golang\t: ${short_path}")

add_custom_target(codegen_go
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto
                    --go_out="${PROJECT_SOURCE_DIR}/static"
                    --plugin="protoc-gen-go=${grpc_go_plugin_path}"
    COMMAND     ${proto_compiler_path} service.proto
                    --go_out="plugins=grpc:${PROJECT_SOURCE_DIR}/static"
                    --plugin="protoc-gen-go=${grpc_go_plugin_path}"
)