cmake_minimum_required(VERSION 3.10)
project(IMAPlw)
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

# set the project name and version
set(PROJECT_NAME IMAPlw)
set(VERSION_NUM 0.1)
project(${PROJECT_NAME} VERSION ${VERSION_NUM})

message(STATUS "${PROJECT_NAME} v${VERSION_NUM} building on ${CMAKE_SYSTEM}")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#include cmake dir for includes
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")


#Boost Library build settings
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
add_definitions(-DBOOST_ALL_DYN_LINK)

#find LibreSSL (LibTLS)
find_package(LibreSSL)
if(NOT LIBRESSL_FOUND)
	#Check whether to get LibreSSL
	if(FETCH_LIBRESSL)
		include(GetLibreSSL)
	else(FETCH_LIBRESSL)
		message(FATAL_ERROR "LibreSSL not found. If you would like to automagically download, try running CMake with \"-DFETCH_LIBRESSL=True\"")
	endif(FETCH_LIBRESSL)
endif(NOT LIBRESSL_FOUND)

#find LibUUID (for uuid generation)
#OSX (& BSD?) have LibUUID functionality built in, and do not require linking
#As such, for Linux-based libUuid is required, otherwise (in case of mistakes,)
#its optional
IF("${CMAKE_SYSTEM}" MATCHES "Linux")
	find_package(LibUuid REQUIRED)
else("${CMAKE_SYSTEM}" MATCHES "Linux")
	find_package(LibUuid)
endif("${CMAKE_SYSTEM}" MATCHES "Linux")

#Find boost library to link
FIND_PACKAGE(Boost 1.54 COMPONENTS locale log log_setup REQUIRED)

#download Miniz for compression
include(GetMiniz)

IF(MINIZ_GOTTEN)
	INCLUDE_DIRECTORIES(${MINIZ_INCLUDE_DIR})
endif(MINIZ_GOTTEN)

IF(LIBUUID_FOUND)
	INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} ${LIBRESSL_INCLUDE_DIR} ${LIBUUID_INCLUDE_DIRS})
ELSE(LIBUUID_FOUND)
	INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} ${LIBRESSL_INCLUDE_DIR})
ENDIF(LIBUUID_FOUND)


#Set output Directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

enable_testing()

#Cache C++
include(CCache)



#Support #include<SocketPool> by adding SocketPool dir to include directories
include_directories(SocketPool)

#Build the SocketPool Static Library to link
add_subdirectory(SocketPool)

#ADD PThread?
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#Build IMAPlw (See src/CMakeLists.txt for more info)
add_subdirectory(src)

add_subdirectory(test)