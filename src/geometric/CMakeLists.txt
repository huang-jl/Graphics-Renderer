CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
project(PT)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

aux_source_directory(. DIR_SRCS)
aux_source_directory(./wrapper WRAPPER_SRCS)

add_library(geometric ${DIR_SRCS} ${WRAPPER_SRCS})

set_target_properties(geometric PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)