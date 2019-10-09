
# protobuf compiler
find_program(proto_compiler_path
    NAMES   protoc.exe protoc
    PATHS   ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/protobuf
            # ${PROJECT_SOURCE_DIR}/tools
            # /usr/local/bin
    NO_DEFAULT_PATH 
)
message(STATUS "using protoc: ${proto_compiler_path}")

# grpc plugin: c++
find_program(grpc_cpp_plugin_path
    NAMES   grpc_cpp_plugin.exe grpc_cpp_plugin
    PATHS   ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/grpc
    NO_DEFAULT_PATH 
)
message(STATUS "using grpc_cpp_plugin: ${grpc_cpp_plugin_path}")

# code generation: c++
add_custom_target(generate_grpc_files_cpp
    # for 'add_custom_command', these files must be generated
    # OUTPUT      static/service.pb.h
    #             static/service.pb.cc
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto
                    --cpp_out=${PROJECT_SOURCE_DIR}/static
    COMMAND     ${proto_compiler_path} service.proto
                    --grpc_out=${PROJECT_SOURCE_DIR}/static
                    --plugin="protoc-gen-grpc=${grpc_cpp_plugin_path}"
)

# grpc plugn: python3
find_program(grpc_python_plugin_path
    NAMES   grpc_python_plugin.exe grpc_python_plugin
    PATHS   ${PROJECT_SOURCE_DIR}/bin
            ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/grpc
    NO_DEFAULT_PATH 
)
message(STATUS "using grpc_python_plugin: ${grpc_python_plugin_path}")

add_custom_target(generate_grpc_files_python
    # for 'add_custom_command', these files must be generated
    # OUTPUT      static/service.pb.go
    #             static/service.pb.go
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto
                    --python_out=${PROJECT_SOURCE_DIR}/static
    COMMAND     ${proto_compiler_path} service.proto
                    --grpc_out=${PROJECT_SOURCE_DIR}/static
                    --plugin="protoc-gen-grpc=${grpc_python_plugin_path}"
)

# grpc plugn: node.js
find_program(grpc_node_plugin_path
    NAMES   grpc_node_plugin.exe grpc_node_plugin
    PATHS   ${PROJECT_SOURCE_DIR}/bin
            ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/grpc
    NO_DEFAULT_PATH 
)
message(STATUS "using grpc_node_plugin: ${grpc_node_plugin_path}")

add_custom_target(generate_grpc_files_nodejs
    # for 'add_custom_command', these files must be generated
    # OUTPUT      static/service.pb.go
    #             static/service.pb.go
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto 
                    --grpc_out=${PROJECT_SOURCE_DIR}/static
                    --plugin="protoc-gen-grpc=${grpc_node_plugin_path}"
)


# grpc plugn: csharp
find_program(grpc_csharp_plugin_path
    NAMES   grpc_csharp_plugin.exe grpc_csharp_plugin
    PATHS   ${PROJECT_SOURCE_DIR}/bin
            ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/grpc
    NO_DEFAULT_PATH 
)
message(STATUS "using grpc_csharp_plugin: ${grpc_csharp_plugin_path}")

add_custom_target(generate_grpc_files_csharp
    # for 'add_custom_command', these files must be generated
    # OUTPUT      static/service.pb...
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto
                    --csharp_out=${PROJECT_SOURCE_DIR}/static
    COMMAND     ${proto_compiler_path} service.proto
                    --grpc_out=${PROJECT_SOURCE_DIR}/static
                    --plugin="protoc-gen-grpc=${grpc_csharp_plugin_path}"
)

# grpc plugn: Go
# $env:GOPATH=pwd
# go get -u github.com/golang/protobuf/protoc-gen-go
find_program(grpc_go_plugin_path
    NAMES   protoc-gen-go.exe protoc-gen-go
    PATHS   ${PROJECT_SOURCE_DIR}/bin
            ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/grpc
    NO_DEFAULT_PATH 
)
message(STATUS "using grpc_go_plugin: ${grpc_go_plugin_path}")

add_custom_target(generate_grpc_files_go
    # for 'add_custom_command', these files must be generated
    # OUTPUT      static/service.pb...
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND     ${proto_compiler_path} --version
    COMMAND     ${proto_compiler_path} service.proto
                    --go_out="${PROJECT_SOURCE_DIR}/static"
                    --plugin="protoc-gen-go=${grpc_go_plugin_path}"
    COMMAND     ${proto_compiler_path} service.proto
                    --go_out="plugins=grpc:${PROJECT_SOURCE_DIR}/static"
                    --plugin="protoc-gen-go=${grpc_go_plugin_path}"
)
