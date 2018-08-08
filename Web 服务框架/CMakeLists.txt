cmake_minimum_required(VERSION 3.11)
project(CppWebFramework)

set(CMAKE_CXX_STANDARD 11)

add_executable(main_http main_http.cpp)
target_link_libraries(main_http pthread boost_system)

add_executable(main_https main_https.cpp)
target_link_libraries(main_https pthread boost_system ssl crypto)
