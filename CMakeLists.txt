cmake_minimum_required(VERSION 3.20)
project(Cmysql)

set(CMAKE_CXX_STANDARD 14)
include_directories( /opt/homebrew/Cellar/mysql/8.0.26/include/mysql /opt/homebrew/Cellar/mysql/8.0.26/lib)
include_directories(/opt/homebrew/Cellar/boost/1.76.0)
link_libraries("/opt/homebrew/Cellar/mysql/8.0.26/lib/libmysqlclient.dylib")
add_executable(Findforeignkey forclass.cpp)
