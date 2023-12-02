#pragma once
#include <iostream>
#include <string>
#include <cstdint>
#include<time.h>
class TimeStamp
{
public:
    TimeStamp();
	//防止函数发生隐式转换
    explicit TimeStamp(int64_t microSecondsSinceEpoch);
	//获取当前的时间戳
    static TimeStamp now();
	//将时间戳格式化输出
    std::string toString()const;

private:
	//当前时间戳
	int64_t microSecondsSinceEpoch_;
};
