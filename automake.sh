#!/bin/bash

# 删除 lib、build 和 bin 目录下的所有文件
echo "Deleting files in lib directory..."
rm -rf lib/*

echo "Deleting files in build directory..."
rm -rf build/*

echo "Deleting files in bin directory..."
rm -rf bin/*

# 执行 cmake
echo "Running cmake..."
cmake -S . -B build

# 切换到 build 目录
cd build

# 执行 make 构建项目
echo "Building project..."
make

echo "Build completed."
echo "------------------------------------------------------------------"
cd ..

cd ./bin

# 获取当前目录下所有可执行文件的列表
./Sylar 

# 遍历可执行文件列表，并执行每个文件


# 退出 bin 目录
cd ..