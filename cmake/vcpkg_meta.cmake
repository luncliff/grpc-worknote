cmake_minimum_required(VERSION 3.13)

if(NOT ${CMAKE_TOOLCHAIN_FILE} MATCHES vcpkg.cmake)
    message(FATAL_ERROR "requires vcpkg.cmake for CMAKE_TOOLCHAIN_FILE")
endif()

message(STATUS "using vcpkg\t: ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
add_library(vcpkg_meta INTERFACE)

target_include_directories(vcpkg_meta
INTERFACE
    ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
)
if(CMAKE_BUILD_TYPE MATCHES Rel) # for release
    target_link_directories(vcpkg_meta
    INTERFACE
        ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
    )
else() # for debug
    target_link_directories(vcpkg_meta
    INTERFACE
        ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib
    )
endif()
