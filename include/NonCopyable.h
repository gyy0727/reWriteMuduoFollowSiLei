/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-19 19:23:23
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-20 19:20:16
 * @FilePath: /桌面/myModuo/include/NonCopyable.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*************************************************************************
  > File Name: noncopyable.h
  > Author: muqiu0614
  > Mail: 3155833132@qq.com
  > Created Time: 2023年11月19日 星期日 19时21分05秒
 ************************************************************************/
#pragma once
#include <iostream>
using namespace std;
class NonCopyable
{
	public:
		NonCopyable(const NonCopyable &) = delete;
		NonCopyable &operator=(const NonCopyable &) = delete;
	protected:
		NonCopyable()=default;
		~NonCopyable()=default;
};
