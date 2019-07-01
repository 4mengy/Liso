cmake_minimum_required(VERSION 3.13)
project(liso)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)


add_executable(liso
        main.cpp
        AsyncLogging.cpp
        AsyncLogging.h
        LogFile.cpp
        LogFile.h
        LogHandler.h
        FixedBuf.h
        NoneCopyable.h)

target_link_libraries(liso "-Wl,-no-as-needed -ltcmalloc -lprofiler")
