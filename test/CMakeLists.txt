cmake_minimum_required(VERSION 3.10)
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

add_executable(miniz_test miniz_test.cpp)

IF(MINIZ_GOTTEN)
	target_link_libraries(miniz_test PRIVATE ${MINIZ_LIBRARIES} ${Boost_LIBRARIES})
	INCLUDE_DIRECTORIES(${MINIZ_INCLUDE_DIR})
ELSE(MINIZ_GOTTEN)
	message(STATUS "Miniz not found")
endif(MINIZ_GOTTEN)


target_compile_features(miniz_test PUBLIC cxx_std_17)
