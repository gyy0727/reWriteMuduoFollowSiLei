#!/bin/bash
###
 # @Author: Gyy0727 3155833132@qq.com
 # @Date: 2023-11-27 18:25:11
 # @LastEditors: Gyy0727 3155833132@qq.com
 # @LastEditTime: 2023-11-27 18:25:34
 # @FilePath: /桌面/myModuo/automake.sh
 # @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
### 

cd ./bin 
rm * -rf
cd .. 
cd ./lib
rm * -rf
cd ..
cd ./build
rm * -rf 
cmake ..
make