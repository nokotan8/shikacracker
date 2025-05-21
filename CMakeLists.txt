cmake_minimum_required(VERSION 3.10)
project(shikacracker)
set(CMAKE_CXX_STANDARD 17)
find_package(OpenSSL REQUIRED)
add_executable(shikacracker 
    src/main.cpp
)
target_link_libraries(shikacracker 
    OpenSSL::Crypto
)
