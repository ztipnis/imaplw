cmake_minimum_required(VERSION 3.10)
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory and run CMake from there. You may need to remove CMakeCache.txt." )
endif()


add_subdirectory(IMAP)

add_executable(IMAPlw IMAP.cpp)


target_link_libraries(IMAPlw libIMAPlw ${Boost_LIBRARIES})

# target_link_libraries(IMAPlw gtest gmock gtest_main)
# add_test(SimpleTest SimpleTest)


target_compile_features(IMAPlw PUBLIC cxx_std_17)
