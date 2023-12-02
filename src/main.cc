/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 13:29:48
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-21 13:39:30
 * @FilePath: /桌面/myModuo/src/main.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/TimeStamp.h"
#include<iostream>
#include <iterator>
#include<string>
int main(){
    TimeStamp a;
    std::cout << a.now().toString() << std::endl;
    return 0;
}