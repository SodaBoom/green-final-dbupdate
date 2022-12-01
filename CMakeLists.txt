cmake_minimum_required(VERSION 3.10)
project(mariadb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

add_executable(to_collect_energy to_collect_energy.cc)
target_link_libraries(to_collect_energy mariadbcpp)

add_executable(total_energy total_energy.cc)
target_link_libraries(total_energy mariadbcpp)
