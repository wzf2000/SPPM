CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(PROJECT CXX)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -O2")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
ENDIF()

IF(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \"-Xpreprocessor -fopenmp -lomp\"")
ENDIF()

ADD_SUBDIRECTORY(deps/vecmath)

SET(PROJECT_SOURCES
        src/image.cpp
        src/main.cpp
        src/renderer.cpp
        src/mesh.cpp
        src/scene_parser.cpp
        src/kdtree.cpp
        src/math.cpp
        src/objectkdtree.cpp)

SET(PROJECT_INCLUDES
        include/camera.hpp
        include/group.hpp
        include/hit.hpp
        include/image.hpp
        include/light.hpp
        include/material.hpp
        include/mesh.hpp
        include/object3d.hpp
        include/plane.hpp
        include/ray.hpp
        include/scene_parser.hpp
        include/sphere.hpp
        include/transform.hpp
        include/triangle.hpp
        include/tile.hpp
        include/renderer.hpp
        include/math.hpp
        include/kdtree.hpp
        include/hitpoint.hpp
        include/texture.hpp
        include/objectkdtree.hpp
        include/disc.hpp
        )

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_INCLUDES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
