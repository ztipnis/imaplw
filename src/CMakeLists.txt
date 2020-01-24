cmake_minimum_required(VERSION 3.10)
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

add_executable(IMAPlw IMAP.cpp)

target_link_libraries(IMAPlw PRIVATE SocketPool LibreSSL::TLS ${Boost_LIBRARIES})


#Link LibUUID if found otherwise don't
IF(LIBUUID_FOUND)
	target_link_libraries(IMAPlw PRIVATE ${LIBUUID_LIBRARIES})
ENDIF(LIBUUID_FOUND)


IF(MINIZ_GOTTEN)
	target_link_libraries(IMAPlw PRIVATE ${MINIZ_LIBRARIES})
	INCLUDE_DIRECTORIES(${MINIZ_INCLUDE_DIR})
ELSE(MINIZ_GOTTEN)
	message(STATUS "Miniz not found")
endif(MINIZ_GOTTEN)

# target_link_libraries(IMAPlw gtest gmock gtest_main)
# add_test(SimpleTest SimpleTest)


target_compile_features(IMAPlw PUBLIC cxx_std_17)
