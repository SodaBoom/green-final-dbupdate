cmake_minimum_required(VERSION 3.10)
project(mariadb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
add_executable(${PROJECT_NAME} main.cc)
target_link_libraries(${PROJECT_NAME} mariadbcpp)
