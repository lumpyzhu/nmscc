file(GLOB_RECURSE SRC "*.cxx")
add_executable(sfc-test main.cxx ${SRC})
target_link_libraries(sfc-test sfc)
