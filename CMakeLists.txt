cmake_minimum_required( VERSION 3.9 )
project( CppUtils CXX C )

#compiler options
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

set( CPPUTIL_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src" ) 
set( CPPUTIL_INC_DIR "${PROJECT_SOURCE_DIR}/inc" )
set( CPPUTIL_TEST_DIR "${PROJECT_SOURCE_DIR}/tests" )


file( GLOB CPPUTIL_SOURCES ${CPPUTIL_SOURCE_DIR}/*.cc ) 
file( GLOB CPPUTIL_HEADERS ${CPPUTIL_INC_DIR}/*.h )

set( CPPUTIL_BUILD_TESTS OFF CACHE BOOL "Should build Unit Tests" )
add_library( CppUtils STATIC ${CPPUTIL_SOURCES} ${CPPUTIL_HEADERS} )
target_compile_features( CppUtils PRIVATE cxx_std_11 )
include_directories( ${CPPUTIL_INC_DIR} )

if( ${CPPUTIL_BUILD_TESTS} )
  add_subdirectory( tests )
endif( ${CPPUTIL_BUILD_TESTS} ) 

