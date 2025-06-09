cmake_minimum_required(VERSION 3.10)

project(shikacracker)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCL REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Werror)
endif()

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/help.cpp
    src/mask_attack.cpp
    src/charsets.cpp
    src/opencl_setup.cpp
)

target_link_libraries(${PROJECT_NAME}     
    PRIVATE OpenCL::OpenCL
)

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE CL_TARGET_OPENCL_VERSION=200
    PRIVATE CL_HPP_TARGET_OPENCL_VERSION=200
)
