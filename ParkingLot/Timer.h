#pragma once
#include <chrono>
#include <iostream>
#include <string>
class Timer
{
private:
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<float> duration;
public:
	Timer();
	std::string elapsed();
	~Timer();
};