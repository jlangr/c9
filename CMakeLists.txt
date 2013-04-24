project(Extras)
cmake_minimum_required(VERSION 2.6)

include_directories($ENV{CPPUTEST_HOME}/include)
link_directories($ENV{CPPUTEST_HOME}/lib) 

add_definitions(-g -std=c++0x)

set(CMAKE_CXX_FLAGS "${CMAXE_CXX_FLAGS} -Wall")
set(sources)
set(testSources WorkTest.cpp)
add_executable(utest testmain.cpp ${testSources} ${sources})

target_link_libraries(utest CppUTest)
target_link_libraries(utest pthread)

