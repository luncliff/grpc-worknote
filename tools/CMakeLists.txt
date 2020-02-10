
add_executable(rpc_service
    rpc_service.cpp  
)
set_target_properties(rpc_service
PROPERTIES
    CXX_STANDARD 17
)
target_link_libraries(rpc_service
PRIVATE
    rpc_impl
)

if(WIN32)
    target_compile_definitions(rpc_service
    PRIVATE
        _WIN32_WINNT=0x0A00 # be explicit
    )
endif()
