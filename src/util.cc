/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2024-03-01 13:53:35
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2024-03-20 20:00:42
 * @FilePath: /sylar/src/util.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/util.h"






pid_t GetThreadId() {
    return syscall(SYS_gettid);
}



uint64_t GetCurrentMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000ul  + tv.tv_usec / 1000;
}

uint64_t GetCurrentUS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 * 1000ul  + tv.tv_usec;
}
