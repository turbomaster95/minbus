cmake_minimum_required(VERSION 3.16)
project(minbus LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(msgpack-cxx REQUIRED)
find_package(cppzmq REQUIRED)

add_library(minbus INTERFACE)

target_include_directories(minbus INTERFACE libminbus/include)
target_link_libraries(minbus INTERFACE msgpack-cxx cppzmq)

add_subdirectory(minbusd)
add_subdirectory(tests)
