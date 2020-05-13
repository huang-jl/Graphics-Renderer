CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(PT)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

ADD_SUBDIRECTORY(deps/vecmath)

SET(PT_SOURCE
        src/Utils.cpp
        src/main.cpp
        src/BVH.cpp)

SET(PT_INCLUDE
    include/Utils.hpp
    include/Ray.hpp
    include/Hitable.hpp
    include/HitableList.hpp
    include/Sphere.hpp
    include/Camera.hpp
    include/Material.hpp
    include/BVH.hpp
    include/Texture.hpp
    include/Perlin.hpp
    include/Light.hpp)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "-std=c++11 -O2 ${CMAKE_CXX_FLAGS}")
MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE(${PROJECT_NAME} ${PT_SOURCE} ${PT_INCLUDE})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
