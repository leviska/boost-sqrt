cmake_minimum_required(VERSION 3.16)

project(boost-sqrt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}../boost_1_76_0)
set(BOOST_ROOT:PATHNAME ${CMAKE_CURRENT_SOURCE_DIR}../boost_1_76_0)
set(Boost_NO_SYSTEM_PATHS TRUE)
set(Boost_NO_BOOST_CMAKE TRUE)

link_directories(/usr/local/lib)

find_package(Boost 1.76.0 REQUIRED COMPONENTS unit_test_framework)
find_package(benchmark REQUIRED)
find_package(GMP REQUIRED)

add_executable(sqrt_test test.cpp)
add_executable(sqrt_bench bench.cpp)

target_compile_options(sqrt_test PRIVATE -Wfatal-errors -g)
target_compile_options(sqrt_bench PRIVATE -Wfatal-errors -O2)

target_compile_features(sqrt_test PRIVATE cxx_std_17)
target_compile_features(sqrt_bench PRIVATE cxx_std_17)

target_link_libraries(sqrt_test Boost::unit_test_framework gmp)
target_link_libraries(sqrt_bench benchmark::benchmark gmp)
