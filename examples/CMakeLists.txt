CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(LOLTOML-EXAMPLES)

INCLUDE_DIRECTORIES(BEFORE
    ${CMAKE_SOURCE_DIR}/include
    ../include
)

SET(CMAKE_CXX_COMPILER clang++)
SET(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-weak-vtables -Wno-padded")

ADD_EXECUTABLE(dnt_parser dnt_parser.cpp)

# The other examples do not yet implement the new handler_t::symbol() method.
##ADD_EXECUTABLE(simple simple.cpp)
##
##ADD_EXECUTABLE(roundtrip roundtrip.cpp)
##
##ADD_EXECUTABLE(toml-test-decoder toml-test-decoder.cpp)
##TARGET_LINK_LIBRARIES(toml-test-decoder kora-util)
##
##ADD_EXECUTABLE(dom dom.cpp)
##TARGET_LINK_LIBRARIES(dom kora-util)
