CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(ffmpeg_demo)
AUX_SOURCE_DIRECTORY(. SOURCE_FILES)
SET(FFMPEG_LIBS
    libavcodec.so
    libavfilter.so
    libavformat.so
    libavutil.so
    libswresample.so
    libswscale.so
)
FIND_PACKAGE(OpenCV REQUIRED)
IF(NOT ${Opencv_FOUND})
    MESSAGE(FATAL_ERROR "OpenCV not found")
ENDIF()
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
ADD_DEFINITIONS(-std=c++11)
ADD_EXECUTABLE(ffmpeg_demo ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(ffmpeg_demo ${FFMPEG_LIBS} ${OpenCV_LIBS})
