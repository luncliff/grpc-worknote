cmake_minimum_required(VERSION 3.13)

add_library(vcpkg_meta INTERFACE)
if(${CMAKE_TOOLCHAIN_FILE} MATCHES vcpkg.cmake)
    message(STATUS "Vcpkg")
    message(STATUS "  Path      \t: ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
    message(STATUS)
    target_include_directories(vcpkg_meta
    INTERFACE
        ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
    )
    target_link_directories(vcpkg_meta
    INTERFACE
        ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
    )
endif()
