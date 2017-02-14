
cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-std=c++14")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

aux_source_directory(src sources)
include_directories(include . )

find_package(GTest REQUIRED)

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost REQUIRED  program_options COMPONENTS)

include_directories(${Boost_INCLUDE_DIRS})

add_executable( gpack ${sources} )

target_link_libraries(gpack GTest::GTest GTest::Main ${Boost_LIBRARIES})
