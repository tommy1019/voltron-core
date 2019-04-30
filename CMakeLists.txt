CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT(voltron-core)

SET(TESTING "OFF" CACHE STRING "Disable compiling of zed and can components")

file(GLOB_RECURSE sourcesC ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE sourcesCPP ${CMAKE_SOURCE_DIR}/src/*.cpp)

IF(NOT TESTING)
    find_package(ZED 2 REQUIRED)
    find_package(CUDA ${ZED_CUDA_VERSION} EXACT REQUIRED)

    include_directories(${CUDA_INCLUDE_DIRS})
    include_directories(${ZED_INCLUDE_DIRS})

    link_directories(${ZED_LIBRARY_DIR})
    link_directories(${CUDA_LIBRARY_DIRS})

    add_definitions(-DCOMPILE_ALL)
ENDIF(NOT TESTING)

ADD_EXECUTABLE(voltron-core ${sourcesC} ${sourcesCPP})

IF(APPLE)
    SET(LIBS )
ELSE(APPLE)
    SET(LIBS m rt pthread)
ENDIF(APPLE)

IF(NOT TESTING)
    add_definitions(-std=c++11 -g -O3)

    SET(LIBS ${ZED_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY} ${CUDA_NPP_LIBRARIES_ZED} m rt pthread)
ENDIF(NOT TESTING)

TARGET_LINK_LIBRARIES(voltron-core ${LIBS})
