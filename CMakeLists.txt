cmake_minimum_required(VERSION 3.16)

project(boost-sqrt VERSION 1.0 LANGUAGES CXX)

find_package(Boost 1.76.0 REQUIRED COMPONENTS unit_test_framework)
find_package(benchmark REQUIRED)

add_compile_options(-Wfatal-errors -O2)

add_executable(sqrt_test test.cpp)
add_executable(sqrt_bench bench.cpp)

target_compile_features(sqrt_test PRIVATE cxx_std_17)
target_compile_features(sqrt_bench PRIVATE cxx_std_17)

target_link_libraries(sqrt_test Boost::unit_test_framework)
target_link_libraries(sqrt_bench benchmark::benchmark)
