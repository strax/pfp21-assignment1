cmake_minimum_required(VERSION 3.17)
project(assignment1)

enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_VERBOSE_MAKEFILE True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fexceptions -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fverbose-asm")

string(REPLACE "-O2" "-O3" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

add_executable(vbyteenc src/vbyteenc.cpp)
add_executable(vbytedec src/vbytedec.cpp)
add_executable(vbyteencsorted src/vbyteencsorted.cpp)
add_executable(vbyteenc-ifstream src/vbyteenc-ifstream.cpp)
add_executable(vbyteenc-ostreambufiterator src/vbyteenc-ostreambufiterator.cpp)
add_executable(vbytedump src/vbytedump.cpp)
add_executable(proximityintersection src/proximityintersection.cpp)
add_executable(vbinfo src/vbinfo.cpp)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
)
