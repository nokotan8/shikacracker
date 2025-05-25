cmake_minimum_required(VERSION 3.10)
project(shikacracker)
set(CMAKE_CXX_STANDARD 17)
find_package(OpenSSL REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Werror)
endif()
add_executable(shikacracker 
    src/main.cpp
    src/helpers.cpp
    src/hash.cpp
    src/dict_attack.cpp
    src/mask_attack.cpp
)
target_link_libraries(shikacracker 
    OpenSSL::Crypto
)
