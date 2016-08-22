cmake_minimum_required(VERSION 3.3)
project(a.man)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/external/cmake)

set(SDL_VERSION 2.0.4)
set(GLEW_VERSION 1.13.0)
set(OPENAL_VERSION 1.17.2)
set(OGG_VERSION 1.3.2)
set(VORBIS_VERSION 1.3.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenGL REQUIRED)
set(LIBRARIES ${LIBRARIES} ${OPENGL_LIBRARIES})
include_directories(${OPENGL_INCLUDE_DIR})

find_package(SDL2)

if (NOT SDL2_FOUND)
    add_subdirectory(external/SDL2-${SDL_VERSION})
    set(SDL2_INCLUDE_DIRS external/SDL2-${SDL_VERSION}/include)
    set(SDL2_LIBRARIES SDL2 SDL2main)
endif()

include_directories(${SDL2_INCLUDE_DIRS})
set(LIBRARIES ${LIBRARIES} ${SDL2_LIBRARIES})

find_package(GLEW)

if (NOT GLEW_FOUND)
    add_subdirectory(external/glew-${GLEW_VERSION}/build/cmake)
    set(GLEW_INCLUDE_DIRS external/glew-${GLEW_VERSION}/include)
    set(GLEW_LIBRARIES glew_s)
endif()

include_directories(${GLEW_INCLUDE_DIRS})
set(LIBRARIES ${LIBRARIES} ${GLEW_LIBRARIES})

find_package(OpenAL)

if (NOT OPENAL_FOUND)
    add_subdirectory(external/openal-soft-${OPENAL_VERSION})
    set(OPENAL_INCLUDE_DIR external/openal-soft-${OPENAL_VERSION}/include)

    if(NOT WIN32)
        set(OPENAL_LIBRARY openal)
    else()
        set(OPENAL_LIBRARY OpenAL32)
    endif()
endif()

include_directories(${OPENAL_INCLUDE_DIR})
set(LIBRARIES ${LIBRARIES} ${OPENAL_LIBRARY})

find_package(OggVorbis)

if(NOT OGGVORBIS_FOUND)
    add_subdirectory(external/libogg-${OGG_VERSION})
    add_subdirectory(external/libvorbis-${VORBIS_VERSION})
    include_directories(
            external/libogg-${OGG_VERSION}/include
            external/libvorbis-${VORBIS_VERSION}/include)
    set(LIBRARIES ${LIBRARIES} libogg libvorbisfile libvorbis)
else()
    include_directories(${OGGVORBIS_INCLUDE_DIR})
    set(LIBRARIES ${LIBRARIES} ${OGGVORBIS_LIBRARIES})
endif()

# lodepng
include_directories(external)

if(NOT WIN32)
    add_definitions(-Wall)

    if(CMAKE_BUILD_TYPE MATCHES DEBUG)
        add_definitions(-g -O0)
    else()
        add_definitions(-O2)
    endif()
else()
    add_definitions(-D_CRT_SECURE_NO_WARNINGS /EHsc)
endif()

file(GLOB AMAN_SOURCES
        src/*.cpp
		src/game/*.cpp
        src/graphics/*.cpp
        src/audio/*.cpp
        src/include/*.h
        external/lodepng/lodepng.cpp)

add_executable(a.man ${AMAN_SOURCES})
target_link_libraries(a.man ${LIBRARIES})

# Copy the files needed at runtime to the destination folder
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})
