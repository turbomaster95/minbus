file(GLOB TEST_SRCS "*.cpp")

foreach(test_src ${TEST_SRCS})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE minbus)
    target_compile_options(${test_name} PRIVATE -Wall -Wextra)
endforeach()
