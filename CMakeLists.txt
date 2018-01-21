
cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-std=c++14")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

aux_source_directory(cli cli_sources)
aux_source_directory(test test_sources)

include_directories(include lib/json_parser )

find_package(GTest REQUIRED)

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost REQUIRED  program_options COMPONENTS)

include_directories(${Boost_INCLUDE_DIRS})

add_executable( gpack_test ${test_sources} )
target_link_libraries(gpack_test GTest::GTest GTest::Main ${Boost_LIBRARIES})

#add_executable( gpack ${cli_sources} )
#target_link_libraries(gpack ${Boost_LIBRARIES})
