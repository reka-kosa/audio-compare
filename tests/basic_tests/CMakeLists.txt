include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runUnitTests
        BasicTests.cpp)

target_link_libraries(runUnitTests gtest gtest_main)
target_link_libraries(runUnitTests src)

find_package(PkgConfig)
# FIND EIGEN
pkg_check_modules( EIGEN3 REQUIRED eigen3 )
include_directories( ${EIGEN3_INCLUDE_DIRS} )