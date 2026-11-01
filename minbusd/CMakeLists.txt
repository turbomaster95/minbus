file(GLOB MINBUSD_SRCS "src/*.cpp")

add_executable(minbusd ${MINBUSD_SRCS})

target_link_libraries(minbusd PRIVATE minbus)

target_compile_options(minbusd PRIVATE -Wall -Wextra)