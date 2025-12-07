cmake_minimum_required(VERSION 3.10)
project(HashMapAnalysis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

add_executable(LRUHarness
        src/LRUHarness/LRUHarness.cpp
        src/implementations/HashNew/HashTableDictionary.cpp
)

target_include_directories(LRUHarness PRIVATE
        src
        src/implementations/HashNew
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(LRUHarness stdc++fs)
endif()

add_executable(LRUTraceGenerator
        src/trace-generators/LRUTraceGenerator.cpp
)


