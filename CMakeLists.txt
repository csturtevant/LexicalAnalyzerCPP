cmake_minimum_required(VERSION 2.8.4)
project(Lexical_Analyzer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES LexicalAnalyzer.cpp)
add_executable(Lexical_Analyzer ${SOURCE_FILES})