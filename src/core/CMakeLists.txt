CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
project(PT)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

aux_source_directory(. DIR_SRCS)

add_library(core ${DIR_SRCS})
target_link_libraries(core vecmath)
set_target_properties(core PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)