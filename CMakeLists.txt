cmake_minimum_required(VERSION 3.17)
project(assignment1)

enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fexceptions -fsanitize=undefined")

add_executable(vbyteenc src/vbyteenc.cpp src/io/memory_mapped_file.h src/vbyte.h)
add_executable(vbytedec src/vbytedec.cpp src/io/memory_mapped_file.h src/vbyte.h)
add_executable(vbyteencsorted src/vbyteencsorted.cpp src/io/memory_mapped_file.h src/vbyte.h)
add_executable(vbytedump src/vbytedump.cpp src/io/memory_mapped_file.h src/vbyte.h)
add_executable(proximityintersection src/proximityintersection.cpp)
add_executable(vbinfo src/vbinfo.cpp src/io/memory_mapped_file.h src/vbyte.h)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
)
