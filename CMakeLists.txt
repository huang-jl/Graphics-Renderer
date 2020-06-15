CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
PROJECT(PT)

SET(CMAKE_CXX_STANDARD 11)
OPTION(DEBUG "options for debug" OFF)
IF(DEBUG)
    message("Debug Build Type")
    ADD_COMPILE_DEFINITIONS(DEBUG)
    ADD_COMPILE_OPTIONS(-Wall)
    SET(CMAKE_BUILD_TYPE DEBUG)
ELSE()
    message("Release Build Type")
    SET(CMAKE_BUILD_TYPE Release)
    ADD_COMPILE_OPTIONS(-O2 -Wall)
ENDIF()

set(LIBS core geometric scene utils)

ADD_SUBDIRECTORY(deps/vecmath)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/geometric/wrapper)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/deps/vecmath/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/deps)

foreach(lib ${LIBS})
    message(${CMAKE_CURRENT_SOURCE_DIR}/src/${lib})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/${lib})
endforeach()

foreach(lib ${LIBS})
    ADD_SUBDIRECTORY(src/${lib})
endforeach()


SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

message(*********)
get_property(p TARGET utils PROPERTY INCLUDE_DIRECTORIES)
foreach(d ${p})
    message(${d})
endforeach()

ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath ${LIBS})