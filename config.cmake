set(CMAKE_CXX_STANDARD 20)
project(sfc)

# cxx
add_compile_options("-W")
add_compile_options("-pthread")
add_link_options("-pthread")
include_directories(src)

# dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
