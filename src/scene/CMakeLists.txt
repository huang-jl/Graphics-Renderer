CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
project(PT)

aux_source_directory(. DIR_SRCS)

add_library(scene ${DIR_SRCS})
set_target_properties(scene PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)