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
	include(ExternalProject)
	ExternalProject_Add(LibreSSL
    	URL https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.0.2.tar.gz 
	)
	ExternalProject_Get_Property(LibreSSL INSTALL_DIR)
	set(LIBRESSL_INCLUDE_DIR ${INSTALL_DIR}/include CACHE PATH "LibreSSL Include Dir")
	set(LIBRESSL_CRYPTO_LIBRARY ${INSTALL_DIR}/lib/libcrypto.a CACHE INTERNAL "LibreSSL CRYTO Library Path")
	set(LIBRESSL_SSL_LIBRARY ${INSTALL_DIR}/lib/libssl.a CACHE INTERNAL "LibreSSL SSL Library Path")
	set(LIBRESSL_TLS_LIBRARY ${INSTALL_DIR}/lib/libtls.a CACHE INTERNAL "LibreSSL TLS Library Path")
	set(LIBRESSL_LIBRARIES ${LIBRESSL_CRYPTO_LIBRARY} ${LIBRESSL_SSL_LIBRARY} ${LIBRESSL_TLS_LIBRARY})
	mark_as_advanced(LIBRESSL_INCLUDE_DIR LIBRESSL_LIBRARIES LIBRESSL_CRYPTO_LIBRARY LIBRESSL_SSL_LIBRARY LIBRESSL_TLS_LIBRARY)
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

target_link_libraries(IMAPlw PRIVATE SocketPool ${LIBRESSL_LIBRARIES} ${Boost_LIBRARIES})



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