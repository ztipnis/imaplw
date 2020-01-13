cmake_minimum_required(VERSION 3.10)
list(APPEND CMAKE_LIBRARY_PATH /usr/local)
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

# set the project name and version
set(PROJECT_NAME IMAPlw)
set(VERSION_NUM 0.1)
project(${PROJECT_NAME} VERSION ${VERSION_NUM})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


FIND_PACKAGE( Boost COMPONENTS locale REQUIRED )
find_package(LibreSSL REQUIRED)


INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(IMAPlw IMAP.cpp)
target_link_libraries(IMAPlw SocketPool LibreSSL::TLS ${Boost_LIBRARIES})
