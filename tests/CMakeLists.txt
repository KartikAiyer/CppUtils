cmake_minimum_required( VERSION 3.0.0 )
project ( CppUtilTests CXX C )

# Add Google Test
set( GTEST_DIR "${PROJECT_SOURCE_DIR}/gtest" )
add_subdirectory( ${GTEST_DIR} )
include_directories( "${GTEST_DIR}/googletest/include" )

set( CPPUTIL_TEST_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src" )
file( GLOB CPPUTIL_TEST_SOURCES "${CPPUTIL_TEST_SOURCE_DIR}/*.cc"
                                "${CPPUTIL_TEST_SOURCE_DIR}/*.h")

add_executable( tests ${CPPUTIL_TEST_SOURCES} )
target_link_libraries( tests gtest CppUtils )
target_compile_features( tests PRIVATE cxx_std_11 )

