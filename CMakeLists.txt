cmake_minimum_required(VERSION 3.10)
project(IMAPlw)
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

#Set output Directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

enable_testing()


#include cmake dir for includes
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")

#Cache C++
include(CCache)

#Support #include<SocketPool> by adding SocketPool dir to include directories
include_directories(SocketPool)

#Build the SocketPool Static Library to link
add_subdirectory(SocketPool)

#Build IMAPlw (See src/CMakeLists.txt for more info)
add_subdirectory(src)