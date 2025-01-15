cmake_minimum_required(VERSION 3.17)
project(test-frame)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/local/include)

link_directories(/usr/local/lib)

add_executable(test-frame
        video_processor.cpp
        jpeg_encoder.cpp
        memory_logger.cpp
        main.cpp
        )


find_library(AVFORMAT_LIB avformat HINTS /usr/local/lib)
find_library(AVCODEC_LIB avcodec HINTS /usr/local/lib)
find_library(AVUTIL_LIB avutil HINTS /usr/local/lib)
find_library(SWSCALE_LIB swscale HINTS /usr/local/lib)

find_path(AVFORMAT_INCLUDE_DIRS avformat.h HINTS /usr/local/include)
find_path(AVCODEC_INCLUDE_DIRS avcodec.h HINTS /usr/local/include)
find_path(AVUTIL_INCLUDE_DIRS avutil.h HINTS /usr/local/include)
find_path(SWSCALE_INCLUDE_DIRS swscale.h HINTS /usr/local/include)

target_link_libraries(test-frame
        avformat
        avcodec
        avutil
        swscale
        pthread
        )
