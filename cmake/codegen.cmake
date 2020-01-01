
set(VCPKG_TOOL_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools)

# protobuf compiler
find_program(proto_compiler_path
    NAMES   protoc.exe protoc
    PATHS   ${VCPKG_TOOL_DIR}/protobuf
    # NO_DEFAULT_PATH 
)
file(RELATIVE_PATH short_path ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET} ${proto_compiler_path})
message(STATUS "  protoc\t: ${short_path}")

include(codegen_cpp)
include(codegen_python)
include(codegen_nodejs)
include(codegen_csharp)

if (DEFINED $ENV{GOPATH})
    include(codegen_go)
endif()
