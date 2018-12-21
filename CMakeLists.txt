cmake_minimum_required(VERSION 3.13)
project(audio_compare)

set(CMAKE_CXX_STANDARD 11)

# If you want to set up additional compiler-related settings, specify them in CMAKE_CXX_FLAGS:
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall")

# HEADER SEARCH PATHS
include_directories(.)

add_executable(audio_compare
        main.cpp)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)


find_package(PkgConfig)

# FIND BOOST
find_package(Boost REQUIRED COMPONENTS filesystem)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(audio_compare ${Boost_LIBRARIES})
    target_link_libraries(runUnitTests ${Boost_LIBRARIES})
endif()

# FIND GSTREAMER
pkg_check_modules(GST REQUIRED gstreamer-1.0>=1.4
        gstreamer-sdp-1.0>=1.4
        gstreamer-video-1.0>=1.4
        gstreamer-app-1.0>=1.4)

# Link gstreamer to project
link_directories(audio_compare ${GST_LIBRARY_DIRS} )
target_include_directories(audio_compare PRIVATE ${GST_INCLUDE_DIRS} )
target_compile_options(audio_compare PRIVATE ${GST_CFLAGS} )
target_link_libraries(audio_compare ${GST_LIBRARIES} )

# Link gtreamer to gtest
link_directories(runUnitTests ${GST_LIBRARY_DIRS} )
target_include_directories(runUnitTests PRIVATE ${GST_INCLUDE_DIRS} )
target_compile_options(runUnitTests PRIVATE ${GST_CFLAGS} )
target_link_libraries(runUnitTests ${GST_LIBRARIES} )

# FIND GTK
if (PKG_CONFIG_FOUND)
    pkg_check_modules(GTK REQUIRED "gtk+-2.0")
    if (GTK_FOUND)
        target_link_libraries(audio_compare ${GTK_LIBRARIES})
        target_link_libraries(runUnitTests ${GTK_LIBRARIES})
        add_definitions(${GTK_CFLAGS} ${GTK_CFLAGS_OTHER})
    endif()
endif()


# FIND EIGEN
pkg_check_modules( EIGEN3 REQUIRED eigen3 )
include_directories( ${EIGEN3_INCLUDE_DIRS} )


target_link_libraries(audio_compare src)
