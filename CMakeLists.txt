cmake_minimum_required(VERSION 3.17)
project(assignment1)

enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(vbyteenc src/vbyteenc.cpp src/MemoryMappedFile.h src/VByte.h)
add_executable(vbytedec src/vbytedec.cpp src/MemoryMappedFile.h src/VByte.h)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
)
