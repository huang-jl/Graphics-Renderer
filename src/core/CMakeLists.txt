CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
project(PT)

aux_source_directory(. DIR_SRCS)

add_library(core ${DIR_SRCS})
target_link_libraries(core vecmath)
set_target_properties(core PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)