file(GLOB MINBUSD_SRCS "src/*.cpp")

add_executable(minbusd ${MINBUSD_SRCS})

target_include_directories(minbusd PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(minbusd PRIVATE minbus)

target_compile_options(minbusd PRIVATE -Wall -Wextra)
