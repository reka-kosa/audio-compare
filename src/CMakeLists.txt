project(src)

set(HEADER_FILES
        core/Audio.h
        core/AudioStreamer.h
        core/AudioProcessor.h
        imported/MFCC.h
        imported/AffinityPropagation.h
        init/InitProcessing.h)

set(SOURCE_FILES
        core/Audio.cpp
        core/AudioProcessor.cpp
        core/AudioStreamer.cpp
        imported/MFCC.cpp
        imported/AffinityPropagation.cpp
        init/InitProcessing.cpp)



add_library(src STATIC ${SOURCE_FILES} ${HEADER_FILES})

find_package(PkgConfig)

# FIND BOOST
find_package(Boost REQUIRED COMPONENTS filesystem)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(src ${Boost_LIBRARIES})
endif()

# FIND GSTREAMER
pkg_check_modules(GST REQUIRED gstreamer-1.0>=1.4
        gstreamer-sdp-1.0>=1.4
        gstreamer-video-1.0>=1.4
        gstreamer-app-1.0>=1.4)

# Link gstreamer to project
link_directories(src ${GST_LIBRARY_DIRS} )
target_include_directories(src PRIVATE ${GST_INCLUDE_DIRS} )
target_compile_options(src PRIVATE ${GST_CFLAGS} )
target_link_libraries(src ${GST_LIBRARIES} )

# FIND GTK
if (PKG_CONFIG_FOUND)
    pkg_check_modules(GTK REQUIRED "gtk+-2.0")
    if (GTK_FOUND)
        target_link_libraries(src ${GTK_LIBRARIES})
        add_definitions(${GTK_CFLAGS} ${GTK_CFLAGS_OTHER})
    endif()
endif()

# FIND EIGEN
pkg_check_modules( EIGEN3 REQUIRED eigen3 )
include_directories( ${EIGEN3_INCLUDE_DIRS} )