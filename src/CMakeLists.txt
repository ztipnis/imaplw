cmake_minimum_required(VERSION 3.10)
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

#Boost Library build settings
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
add_definitions(-DBOOST_ALL_DYN_LINK)

#Find boost library to link
FIND_PACKAGE(Boost 1.54 COMPONENTS locale log log_setup REQUIRED)

#find LibreSSL (LibTLS)
find_package(LibreSSL)
if(NOT LIBRESSL_FOUND)
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

#Link if found otherwise don't
IF(LIBUUID_FOUND)
	INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} ${LIBRESSL_INCLUDE_DIR} ${LIBUUID_INCLUDE_DIRS})
ELSE(LIBUUID_FOUND)
	INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} ${LIBRESSL_INCLUDE_DIR})
ENDIF(LIBUUID_FOUND)

add_executable(IMAPlw IMAP.cpp)

target_link_libraries(IMAPlw PRIVATE SocketPool LibreSSL::TLS ${Boost_LIBRARIES})



IF(LIBUUID_FOUND)
	target_link_libraries(IMAPlw PRIVATE ${LIBUUID_LIBRARIES})
ENDIF(LIBUUID_FOUND)

# target_link_libraries(IMAPlw gtest gmock gtest_main)
# add_test(SimpleTest SimpleTest)


target_compile_features(IMAPlw PUBLIC cxx_std_17)

set(CMAKE_CXX_FLAGS "-g -std=c++17 -Werror \
                     -Wno-error=variadic-macros \
                     -Werror=implicit-fallthrough"
                    CACHE STRING "compiler flags")