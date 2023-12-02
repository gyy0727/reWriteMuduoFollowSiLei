#设置版本要求
cmake_minimum_required(VERSION 3.0...3.7)
#设置项目名称
project(myMuduo)
#设置可执行文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#查找源文件并输出到SRC
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
#设置c++标准
set(CXX_STANDARD 17)
#设置头文件存放路径
include_directories(${PROJECT_SOURCE_DIR}/include)
#设置库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# add_library(threadPool SHARED ${SRC})
#设置根据哪些文件生成可执行文件
add_executable(myMuduo ${SRC})