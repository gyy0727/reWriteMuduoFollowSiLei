/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-30 08:57:51
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-30 09:06:26
 * @FilePath: /桌面/myModuo/src/CurrentThreadId.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/CurrentThreadId.h"

namespace CurrentThread
{
    __thread int t_cachedTid = 0;   

    void cacheTid()
    {
        if (t_cachedTid == 0)
        {
            // 通过linux系统调用，获取当前线程的tid值
            t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }
}